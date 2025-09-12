using System;
using System.IO;
using System.Windows.Forms;
using System.Threading;


namespace sound_pud
{
    using DTO;

    public partial class Form1 : Form
    {
        private bool _isInitializing = true;

        private GlobalInputListener _listener;
        private SettingsManager _settingsManager;

        public Form1()
        {
            InitializeComponent();
            _settingsManager = new SettingsManager();

            // Проверка, есть ли уже биндинг на -13 (Fallback)
            if (_settingsManager.HasKeyBinding(-13))
            {
                checkBox_all_bind.Checked = true;
            }

            _isInitializing = false;

            _listener = new GlobalInputListener();

           
            // Подписка на события глобального ввода
            _listener.InputReceived += (s, e) =>
            {
                if (e.IsPressed) // KeyDown / MouseDown
                {
                    _settingsManager.GetPlayerForKey(e.Key);
                }
                else // KeyUp / MouseUp
                {
                    _settingsManager.StopKey(e.Key);
                }
            };
     

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            _listener.Start();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            _settingsManager.Stop();
            _listener.Stop(); // выключаем хуки при закрытии
        }

        private void checkBox_all_bind_CheckedChanged(object sender, EventArgs e)
        {
            if (_isInitializing) return; // пропускаем вызов при старте формы

            if (checkBox_all_bind.Checked)
            {
                using (var configForm = new ConfigForm())
                {
                    if (configForm.ShowDialog() == DialogResult.OK)
                    {
                        // Получаем конфиг из окна
                        var config = configForm.GetConfig();

                        // Добавляем в менеджер (бинд на -13)
                        _settingsManager.AddKeyBinding(-13, config);
                    }
                    else
                    {
                        // Если пользователь отменил — возвращаем чекбокс обратно
                        checkBox_all_bind.Checked = false;
                    }
                }
            }
            else
            {
                // Если выключено — очищаем fallback биндинг
                _settingsManager.RemoveKey(-13);
            }
        }

        private void buttonAddKey_Click(object sender, EventArgs e)
        {
            using (var keyForm = new KeyCaptureForm())
            {
                if (keyForm.ShowDialog() != DialogResult.OK)
                    return;

                int key = keyForm.CapturedKey;

                using (var configForm = new ConfigForm())
                {
                    if (configForm.ShowDialog() != DialogResult.OK)
                        return;

                    var config = configForm.GetConfig();

                    // Проверяем, есть ли уже биндинг
                    if (_settingsManager.HasKeyBinding(key))
                    {
                        // Обновляем конфигурацию существующей группы
                        _settingsManager.ReplaceKeyBinding(key, config);
                    }
                    else
                    {
                        // Добавляем новую группу с выбранной клавишей
                        _settingsManager.AddKeyBinding(key, config);
                    }
                }
            }
        }

        private void buttonDelete_Click(object sender, EventArgs e)
        {
            using (var keyForm = new KeyCaptureForm())
            {
                if (keyForm.ShowDialog() != DialogResult.OK)
                    return;

                int key = keyForm.CapturedKey;

                if (_settingsManager.HasKeyBinding(key))
                {
                    // Удаляем биндинг
                    _settingsManager.RemoveKey(key);
                }
                else
                {
                    // Сообщаем, что такого биндинга нет
                    MessageBox.Show($"No binding found for key.", "Info", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

    }

}
