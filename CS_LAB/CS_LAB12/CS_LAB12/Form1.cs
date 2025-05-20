

using System;
using System.ComponentModel;
using System.Linq;
using System.Windows.Forms;

namespace CS_LAB10
{
    public partial class Form1 : Form
    {
        private int[,] matrix1;
        private int[,] matrix2;
        private int[,] resultMatrix;

        private int totalElements;
        private int processedElements = 0;

        public Form1()
        {
            InitializeComponent();
            progressBar1.Minimum = 0;
            progressBar1.Style = ProgressBarStyle.Continuous;

            backgroundWorker1.WorkerReportsProgress = true;
            backgroundWorker1.WorkerSupportsCancellation = true;
            backgroundWorker1.DoWork += backgroundWorker1_DoWork;
            backgroundWorker1.ProgressChanged += BackgroundWorker1_ProgressChanged;
            backgroundWorker1.RunWorkerCompleted += BackgroundWorker1_RunWorkerCompleted;
        }


        private void dataGridView3_CellContentClick(object sender, DataGridViewCellEventArgs e) { }
        private void progressBar1_Click(object sender, EventArgs e) { }
        private void textBox1_TextChanged(object sender, EventArgs e) { }
        private void textBox2_TextChanged(object sender, EventArgs e) { }
        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                matrix1 = ParseMatrixFromTextBox(textBox1.Text);
                matrix2 = ParseMatrixFromTextBox(textBox2.Text);

                if (matrix1.GetLength(1) != matrix2.GetLength(0))
                {
                    MessageBox.Show("Macierze nie mogą zostać pomnożone: liczba kolumn pierwszej macierzy nie jest równa liczbie wierszy drugiej macierzy.",
                                    "Błąd", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                resultMatrix = new int[matrix1.GetLength(0), matrix2.GetLength(1)];

                totalElements = resultMatrix.GetLength(0) * resultMatrix.GetLength(1);
                processedElements = 0;

                progressBar1.Maximum = totalElements;

                backgroundWorker1.RunWorkerAsync();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Błąd: {ex.Message}", "Błąd", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                int size = int.Parse(textBox1.Text);
                if (size <= 0)
                {
                    MessageBox.Show("Wielkość macierzy musi być liczbą całkowitą większą od 0.", "Błąd", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                matrix1 = GenerateIdentityMatrix(size);
                matrix2 = GenerateIdentityMatrix(size);

                textBox1.Text = MatrixToString(matrix1);
                textBox2.Text = MatrixToString(matrix2);

                resultMatrix = new int[matrix1.GetLength(0), matrix2.GetLength(1)];

                totalElements = resultMatrix.GetLength(0) * resultMatrix.GetLength(1);
                processedElements = 0;

                progressBar1.Maximum = totalElements;

                backgroundWorker1.RunWorkerAsync();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Błąd: {ex.Message}", "Błąd", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private int[,] GenerateIdentityMatrix(int size)
        {
            int[,] identityMatrix = new int[size, size];

            for (int i = 0; i < size; i++)
            {
                for (int j = 0; j < size; j++)
                {
                    identityMatrix[i, j] = (i == j) ? 1 : 0;
                }
            }

            return identityMatrix;
        }
        private string MatrixToString(int[,] matrix)
        {
            int rows = matrix.GetLength(0);
            int cols = matrix.GetLength(1);
            string result = "";

            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    result += matrix[i, j].ToString() + " ";
                }
                result = result.Trim() + ",";
            }

            if (result.EndsWith(","))
                result = result.Substring(0, result.Length - 1);

            return result;
        }

        private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {
            int rows1 = matrix1.GetLength(0);
            int cols1 = matrix1.GetLength(1);
            int cols2 = matrix2.GetLength(1);

            for (int i = 0; i < rows1; i++)
            {
                for (int j = 0; j < cols2; j++)
                {
                    resultMatrix[i, j] = 0;
                    for (int k = 0; k < cols1; k++)
                    {
                        if (backgroundWorker1.CancellationPending)
                        {
                            e.Cancel = true;
                            return;
                        }
                        resultMatrix[i, j] += matrix1[i, k] * matrix2[k, j];
                    }

                    processedElements++;
                    backgroundWorker1.ReportProgress(processedElements);
                }
            }
        }

        private void BackgroundWorker1_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            progressBar1.Value = Math.Min(e.ProgressPercentage, progressBar1.Maximum);
        }

        private void BackgroundWorker1_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            if (e.Cancelled)
            {
                MessageBox.Show("Operacja została anulowana.", "Komunikat", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show("Mnożenie macierzy zakończone!", "Komunikat", MessageBoxButtons.OK, MessageBoxIcon.Information);
                DisplayMatrixInDataGridView(dataGridView3, resultMatrix);
            }
        }

        private int[,] ParseMatrixFromTextBox(string input)
        {
            var rows = input.Split(new[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
            int maxCols = rows.Max(r => r.Trim().Split(' ').Length);
            int rowCount = rows.Length;

            int[,] matrix = new int[rowCount, maxCols];

            for (int i = 0; i < rowCount; i++)
            {
                var cols = rows[i].Trim().Split(' ');

                for (int j = 0; j < cols.Length; j++)
                {
                    matrix[i, j] = !string.IsNullOrEmpty(cols[j]) && int.TryParse(cols[j], out int value) ? value : 0;
                }

                for (int j = cols.Length; j < maxCols; j++)
                {
                    matrix[i, j] = 0;
                }
            }

            return matrix;
        }

        private void DisplayMatrixInDataGridView(DataGridView dgv, int[,] matrix)
        {
            int rows = matrix.GetLength(0);
            int cols = matrix.GetLength(1);

            dgv.RowCount = rows;
            dgv.ColumnCount = cols;

            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    dgv[j, i].Value = matrix[i, j];
                }
            }
        }
    }
}

