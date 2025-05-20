using System;
using System.IO;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace CS_LAB07_01
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void textBox1_TextChanged(object sender, EventArgs e){}
        private void textBox2_TextChanged(object sender, EventArgs e){}

        private void button1_Click(object sender, EventArgs e)
        {
            using (FolderBrowserDialog folderBrowser = new FolderBrowserDialog())
            {
                if (folderBrowser.ShowDialog() == DialogResult.OK)
                {
                    textBox1.Text = folderBrowser.SelectedPath;
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            using (FolderBrowserDialog folderBrowser = new FolderBrowserDialog())
            {
                if (folderBrowser.ShowDialog() == DialogResult.OK)
                {
                    textBox2.Text = folderBrowser.SelectedPath;
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(textBox1.Text) || string.IsNullOrWhiteSpace(textBox2.Text))
            {
                MessageBox.Show("Wybierz katalog źródłowy i docelowy.", "Błąd", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            string sourceDirectory = textBox1.Text;
            string targetDirectory = textBox2.Text;

            try
            {
                ProcessDirectory(sourceDirectory, targetDirectory);
                MessageBox.Show("Kopiowanie zakończone!", "Sukces", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Wystąpił błąd: {ex.Message}", "Błąd", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void ProcessDirectory(string sourceDir, string targetDir)
        {
            foreach (string dirPath in Directory.GetDirectories(sourceDir, "*", SearchOption.AllDirectories))
            {
                ProcessFilesInDirectory(dirPath, targetDir);
            }

            ProcessFilesInDirectory(sourceDir, targetDir);
        }

        private void ProcessFilesInDirectory(string currentDir, string targetDir)
        {
            foreach (string filePath in Directory.GetFiles(currentDir, "*.jpg", SearchOption.TopDirectoryOnly))
            {
                CopyFileWithNewName(filePath, targetDir, currentDir);
            }
        }

        private void CopyFileWithNewName(string filePath, string targetDir, string sourceDir)
        {
            FileInfo fileInfo = new FileInfo(filePath);

            DateTime creationTime = fileInfo.CreationTime;
            string datePrefix = creationTime.ToString("yyyyMMddHHmmss");

            string folderName = new DirectoryInfo(sourceDir).Name;
            string newFileName = $"{datePrefix}_{folderName}_{fileInfo.Name}";

            string destinationPath = Path.Combine(targetDir, newFileName);

            fileInfo.CopyTo(destinationPath, overwrite: true);
        }
    }
}
