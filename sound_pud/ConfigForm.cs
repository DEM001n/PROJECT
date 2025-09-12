using System;
using System.IO;
using System.Windows.Forms;

namespace sound_pud
{
    using DTO; // чтобы видеть SoundConfig

    public partial class ConfigForm : Form
    {
        private string _selectedFilePath;

        public ConfigForm()
        {
            InitializeComponent();

            // Настройки по умолчанию
            trackBarVolume.Minimum = 0;
            trackBarVolume.Maximum = 10;
            trackBarVolume.Value = 7; // теперь безопасно

            trackBarMinPercent.Minimum = 0;
            trackBarMinPercent.Maximum = 10;
            trackBarMinPercent.Value = 3;

            checkBoxLoop.Checked = false;

            textBoxFile.ReadOnly = true;
        }

        private void buttonSelectFile_Click(object sender, EventArgs e)
        {
            using (var ofd = new OpenFileDialog())
            {
                ofd.Filter = "Audio files (*.wav)|*.wav";
                ofd.InitialDirectory = Paths.AudioPath; // всегда открывать папку Sounds
                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    _selectedFilePath = ofd.FileName;

                    // Если файл выбран не из папки Sounds — копируем туда
                    string destPath = Path.Combine(Paths.AudioPath, Path.GetFileName(_selectedFilePath));
                    if (!File.Exists(destPath))
                    {
                        File.Copy(_selectedFilePath, destPath);
                    }

                    textBoxFile.Text = Path.GetFileName(_selectedFilePath);
                    _selectedFilePath = destPath;
                }
            }
        }

        private void buttonOk_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(_selectedFilePath))
            {
                MessageBox.Show("Выберите аудиофайл.", "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        // Вот тут главный метод
        public SoundConfig GetConfig()
        {
            return new SoundConfig
            {
                FilePath = Path.GetFileName(_selectedFilePath),
                Volume = trackBarVolume.Value / 10f,         // в диапазон 0..1
                MinPlayInterval = trackBarMinPercent.Value / 10f,
                Loop = checkBoxLoop.Checked
            };
        }
    }
}
