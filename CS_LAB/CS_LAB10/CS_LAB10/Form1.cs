using System;
using System.Linq;
using System.Windows.Forms;
using CS_LIBRARY12;

namespace CS_LAB10
{
    public partial class Form1 : Form
    {
        private int[,] matrix1;
        private int[,] matrix2;
        private int[,] resultMatrix;
        private int totalElements;

        private CustomBackgroundWorker customBackgroundWorker;

        public Form1()
        {
            InitializeComponent();
            progressBar1.Minimum = 0;
            progressBar1.Maximum = 100;
            progressBar1.Style = ProgressBarStyle.Continuous;

            customBackgroundWorker = new CustomBackgroundWorker();
            customBackgroundWorker.ProgressChanged += (progress) =>
                Invoke((Action)(() => progressBar1.Value = Math.Min(progress, progressBar1.Maximum)));
            customBackgroundWorker.OnError += (exception) =>
                MessageBox.Show($"Błąd: {exception.Message}", "Błąd", MessageBoxButtons.OK, MessageBoxIcon.Error);
            customBackgroundWorker.OnCompleted += () =>
            {
                if (dataGridView3.InvokeRequired)
                {
                    dataGridView3.Invoke(new Action(() =>
                    {
                        MessageBox.Show("Mnożenie macierzy zakończone!", "Informacja", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        DisplayMatrixInDataGridView(dataGridView3, resultMatrix);
                    }));
                }
                else
                {
                    MessageBox.Show("Mnożenie macierzy zakończone!", "Informacja", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    DisplayMatrixInDataGridView(dataGridView3, resultMatrix);
                }
            };
        }

        private void dataGridView3_CellContentClick(object sender, DataGridViewCellEventArgs e) { }
        private void progressBar1_Click(object sender, EventArgs e) { }
        private void textBox1_TextChanged(object sender, EventArgs e) { }
        private void textBox2_TextChanged(object sender, EventArgs e) { }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                int size = int.Parse(textBox1.Text);
                if (size <= 0)
                {
                    MessageBox.Show("Rozmiar macierzy musi być większy od 0.", "Błąd", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                matrix1 = GenerateIdentityMatrix(size);
                matrix2 = GenerateIdentityMatrix(size);

                textBox1.Text = MatrixToString(matrix1);
                textBox2.Text = MatrixToString(matrix2);

                resultMatrix = new int[matrix1.GetLength(0), matrix2.GetLength(1)];
                totalElements = resultMatrix.GetLength(0) * resultMatrix.GetLength(1);
                progressBar1.Value = 0;

                customBackgroundWorker.RunWorkerAsync(MatrixOperations.MultiplyMatrices, new object[] { matrix1, matrix2, resultMatrix, totalElements, customBackgroundWorker });

                Timer timer = new Timer { Interval = 100 };
                timer.Tick += (s, args) =>
                {
                    customBackgroundWorker.ProcessQueue();
                    if (progressBar1.Value == progressBar1.Maximum)
                        timer.Stop();
                };
                timer.Start();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Błąd: {ex.Message}", "Błąd", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                matrix1 = ParseMatrixFromTextBox(textBox1.Text);
                matrix2 = ParseMatrixFromTextBox(textBox2.Text);

                if (matrix1.GetLength(1) != matrix2.GetLength(0))
                {
                    MessageBox.Show("Nie można pomnożyć macierzy: liczba kolumn pierwszej macierzy nie jest równa liczbie wierszy drugiej macierzy.",
                                    "Błąd", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                resultMatrix = new int[matrix1.GetLength(0), matrix2.GetLength(1)];
                totalElements = resultMatrix.GetLength(0) * resultMatrix.GetLength(1);
                progressBar1.Value = 0;

                customBackgroundWorker.RunWorkerAsync(MatrixOperations.MultiplyMatrices, new object[] { matrix1, matrix2, resultMatrix, totalElements, customBackgroundWorker });

                Timer timer = new Timer { Interval = 100 };
                timer.Tick += (s, args) =>
                {
                    customBackgroundWorker.ProcessQueue();
                    if (progressBar1.Value == progressBar1.Maximum)
                        timer.Stop();
                };
                timer.Start();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Błąd: {ex.Message}", "Błąd", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
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
    }
}

