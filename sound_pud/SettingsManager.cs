using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;

namespace sound_pud
{
    using DTO;
    using System.Linq;
    using System.Threading;

    internal class SettingsManager
    {
        private const int FallbackKey = -13;

        private List<SoundConfigWithKeys> _configs;
        public SortedDictionary<int, KeyPlayerInfo> FastLookup { get; private set; }


        public SettingsManager()
        {
            LoadSettings();
            BuildFastLookup();
        }

        /// <summary>
        /// Загружает настройки из JSON в объекты.
        /// </summary>
        public void LoadSettings()
        {
            if (File.Exists(Paths.SettingsPath))
            {
                var json = File.ReadAllText(Paths.SettingsPath);
                _configs = JsonConvert.DeserializeObject<List<SoundConfigWithKeys>>(json) ?? new List<SoundConfigWithKeys>();
            }
        }

        /// <summary>
        /// Сохраняет текущие настройки в JSON.
        /// </summary>
        public void SaveSettings()
        {
            var json = JsonConvert.SerializeObject(_configs, Formatting.Indented);
            File.WriteAllText(Paths.SettingsPath, json);
        }

        /// <summary>
        /// Возвращает конфигурацию для клавиши, если есть.
        /// </summary>
        public SoundConfig GetConfigForKey(int keyCode)
        {
            foreach (var cfg in _configs)
            {
                if (cfg.Keys.Contains(keyCode))
                    return cfg.SoundConfig;
            }
            return null;
        }
        public bool HasKeyBinding(int keyCode)
        {
            return _configs.Any(cfg => cfg.Keys.Contains(keyCode));
        }

        /// <summary>
        /// Добавляет клавишу к существующей конфигурации или создаёт новую группу.
        /// </summary>
        public void AddKeyBinding(int keyCode, SoundConfig config)
        {
            var existingGroup = _configs.Find(c => AreConfigsEqual(c.SoundConfig, config));
            if (existingGroup != null)
            {
                if (!existingGroup.Keys.Contains(keyCode))
                    existingGroup.Keys.Add(keyCode);
            }
            else
            {
                _configs.Add(new SoundConfigWithKeys
                {
                    SoundConfig = config,
                    Keys = new List<int> { keyCode }
                });
            }

            if (FastLookup != null && !FastLookup.ContainsKey(keyCode))
                FastLookup[keyCode] = new KeyPlayerInfo { Player = null, TokenSource = null };

        }

        /// <summary>
        /// Убирает клавишу из всех конфигураций. Если группа пустая, удаляет её.
        /// </summary>
        public void RemoveKey(int keyCode)
        {
            foreach (var cfg in _configs.ToArray()) // перебираем копию
            {
                if (cfg.Keys.Remove(keyCode))
                {
                    if (cfg.Keys.Count == 0)
                        _configs.Remove(cfg);

                    break;
                }
            }

            if (FastLookup != null && FastLookup.ContainsKey(keyCode))
            {
                StopKey(keyCode);
                FastLookup.Remove(keyCode);
            }
        }

        /// <summary>
        /// Полностью заменяет конфигурацию для указанной клавиши.
        /// Старое назначение удаляется, и создаётся новое с переданным config.
        /// </summary>
        public void ReplaceKeyBinding(int keyCode, SoundConfig newConfig)
        {
            // 1. Убираем клавишу из всех старых групп
            RemoveKey(keyCode);

            // 2. Добавляем в новую группу
            AddKeyBinding(keyCode, newConfig);

            // 3. Обновляем FastLookup
            if (FastLookup != null)
            {
                // Сброс к "ленивой инициализации"
                FastLookup[keyCode] = new KeyPlayerInfo { Player = null, TokenSource = null };
            }
        }


        /// <summary>
        /// Получает все клавиши для конкретной конфигурации.
        /// </summary>
        public IReadOnlyCollection<int> GetKeysForConfig(SoundConfig config)
        {
            var group = _configs.Find(c => AreConfigsEqual(c.SoundConfig, config));
            return (IReadOnlyCollection<int>)(group?.Keys.AsReadOnly() ?? Array.AsReadOnly(new int[0]));
        }


        /// <summary>
        /// Сравнивает две конфигурации по значимым свойствам.
        /// </summary>
        private bool AreConfigsEqual(SoundConfig a, SoundConfig b)
        {
            if (a == null || b == null) return false;
            return a.FilePath == b.FilePath &&
                   Math.Abs(a.Volume - b.Volume) < 0.001f &&
                   Math.Abs(a.MinPlayInterval - b.MinPlayInterval) < 0.001f &&
                   a.Loop == b.Loop;
        }

        public void BuildFastLookup()
        {
            FastLookup = new SortedDictionary<int, KeyPlayerInfo>();
            foreach (var group in _configs)
            {
                // Сначала добавляем все клавиши с null
                foreach (var key in group.Keys)
                {
                    if (!FastLookup.ContainsKey(key))
                        FastLookup[key] = new KeyPlayerInfo { Player = null, TokenSource = null };
                }
            }
        }

        /// <summary>
        /// Получение SoundPlayerManager по клавише с ленивой инициализацией.
        /// </summary>
        public SoundPlayerManager GetPlayerForKey(int keyCode)
        {
            int lookupKey = FastLookup.ContainsKey(keyCode) ? keyCode : FallbackKey;

            if (!FastLookup.ContainsKey(lookupKey))
                return null;

            var info = FastLookup[lookupKey];

            // Если Player еще не создан — создаем и присваиваем всем клавишам группы
            if (info.Player == null)
            {
                var group = _configs.Find(g => g.Keys.Contains(lookupKey));
                info.Player = new SoundPlayerManager(group.SoundConfig);

                foreach (var k in group.Keys)
                {
                    if (!FastLookup.ContainsKey(k))
                        FastLookup[k] = new KeyPlayerInfo { Player = info.Player, TokenSource = null };
                    else
                        FastLookup[k].Player = info.Player;
                }
            }

            // Теперь создаем новый токен и запускаем воспроизведение
            info.TokenSource?.Cancel();
            info.TokenSource = new CancellationTokenSource();
            info.Player.Play(info.TokenSource.Token);

            return info.Player;
        }


        public void StopKey(int keyCode)
        {
            KeyPlayerInfo info = null;

            // 1. Проверяем обычную клавишу
            if (FastLookup.TryGetValue(keyCode, out var directInfo))
            {
                info = directInfo;
            }
            // 2. Если нет, проверяем fallback (-13)
            else if (FastLookup.TryGetValue(-13, out var fallbackInfo))
            {
                info = fallbackInfo;
            }

            // 3. Если нашли и есть активный токен — отменяем
            if (info?.TokenSource != null && !info.TokenSource.IsCancellationRequested)
            {
                info.TokenSource.Cancel();
                info.TokenSource = null;
            }
        }



        public void Stop()
        {
            // Останавливаем все активные токены
            foreach (var kvp in FastLookup)
            {
                kvp.Value.TokenSource?.Cancel();
                kvp.Value.TokenSource = null;
            }

            // Сохраняем настройки и очищаем структуры
            SaveSettings();
            FastLookup?.Clear();
            _configs?.Clear();
            FastLookup = null;
            _configs = null;
        }
    }
}

