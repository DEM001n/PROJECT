using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using NAudio.Wave;
using NAudio.Wave.SampleProviders;

namespace sound_pud
{
    using DTO;

    public class SoundPlayerManager
    {
        private static readonly Dictionary<string, byte[]> AudioCache = new Dictionary<string, byte[]>();
        private static readonly object CacheLock = new object();

        public SoundConfig Config { get; private set; }
        private byte[] _audioData;

        public SoundPlayerManager(SoundConfig config)
        {
            Config = config ?? throw new ArgumentNullException(nameof(config));
        }

        /// <summary>
        /// Ленивое получение данных аудио.
        /// Загружает из кэша, если уже есть, иначе считывает с диска.
        /// </summary>
        private void EnsureAudioLoaded()
        {
            if (_audioData != null) return; // уже загружено

            lock (CacheLock)
            {
                if (!AudioCache.TryGetValue(Config.FilePath, out _audioData))
                {
                    // Полный путь к файлу в папке AudioPath
                    string fullPath = Path.Combine(Paths.AudioPath, Config.FilePath);

                    if (!File.Exists(fullPath))
                        throw new FileNotFoundException($"Файл аудио не найден: {fullPath}");

                    _audioData = File.ReadAllBytes(fullPath);
                    AudioCache[Config.FilePath] = _audioData; // сохраняем в кэш
                }
                else
                {
                    // Берём из кэша
                    _audioData = AudioCache[Config.FilePath];
                }
            }
        }

        /// <summary>
        /// Проигрывание с использованием ленивой загрузки
        /// </summary>
        public void Play(CancellationToken cancellationToken = default)
        {
            EnsureAudioLoaded();

            Task.Run(() =>
            {
                do
                {
                    using (var ms = new MemoryStream(_audioData))
                    using (var reader = new WaveFileReader(ms))
                    using (var output = new WaveOutEvent())
                    {
                        var sampleChannel = new SampleChannel(reader, true)
                        {
                            Volume = Config.Volume
                        };
                        output.Init(sampleChannel);
                        output.Play();

                        int totalMs = (int)reader.TotalTime.TotalMilliseconds;
                        int minPlayMs = (int)(totalMs * Config.MinPlayInterval);
                        int elapsedMs = 0;

                        while (output.PlaybackState == PlaybackState.Playing)
                        {
                            Thread.Sleep(20);
                            elapsedMs += 21;

                            // Если пришёл сигнал остановки и прошло минимальное время, останавливаем сразу
                            if (cancellationToken.IsCancellationRequested && elapsedMs >= minPlayMs)
                            {
                                output.Stop();
                                break;
                            }
                        }
                    }
                } while (Config.Loop && !cancellationToken.IsCancellationRequested);
            }, cancellationToken);
        }
    }
}
