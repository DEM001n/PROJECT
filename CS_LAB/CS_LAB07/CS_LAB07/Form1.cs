using System;
using System.Windows.Forms;

namespace CS_LAB07
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e){}

        private void button1_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(textBox1.Text) || string.IsNullOrWhiteSpace(textBox2.Text))
            {
                MessageBox.Show("Please provide paths for the CSV file and HTML output.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            try
            {
                Converter.Program.convertCsvtoHtml(textBox1.Text, textBox2.Text);
                MessageBox.Show("Conversion completed successfully!", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"An error occurred during conversion: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog openFileDialog = new OpenFileDialog())
            {
                openFileDialog.Filter = "CSV files (*.csv)|*.csv|All files (*.*)|*.*";
                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    textBox1.Text = openFileDialog.FileName;
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            using (SaveFileDialog saveFileDialog = new SaveFileDialog())
            {
                saveFileDialog.Filter = "HTML files (*.html)|*.html|All files (*.*)|*.*";
                if (saveFileDialog.ShowDialog() == DialogResult.OK)
                {
                    textBox2.Text = saveFileDialog.FileName;
                }
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e){}
        private void textBox2_TextChanged(object sender, EventArgs e){}
    }
}

namespace Converter
{
    class Program
    {
        public static void nagłówekPliku(System.IO.StreamWriter writer)
        {
            writer.WriteLine("<!DOCTYPE html>\n<html>\n<head>\n<style>\ntable {font-family: arial, sans-serif;border-collapse: collapse;width: 100%;}\ntd, th {border: 1px solid #dddddd;text-align: left;padding: 8px;}\ntr:nth-child(even) {background-color: #dddddd;}\n</style>\n</head>\n<body>");
        }

        public static void koniecPliku(System.IO.StreamWriter writer)
        {
            writer.WriteLine("</table>\n</body>\n</html>");
        }

        public static void wieszNagłówkowy(System.IO.StreamWriter writer, string text)
        {
            writer.WriteLine($"<h2>{text}</h2>\n<table>");

        }

        public static void stopkaPliku(System.IO.StreamWriter writer, string[] text)
        {

            writer.WriteLine("<tr>");

            foreach (var element in text)
            {
                writer.WriteLine($"<td>{element}</td>");
            }
            writer.WriteLine("</tr>");
        }

        public static bool wierszeDanych(System.IO.StreamWriter writer, string[] text)
        {

            writer.WriteLine("</tr>");
            foreach (var element in text)
            {
                writer.WriteLine($"<th>{element}</th>");
            }
            writer.WriteLine("</tr>");

            return false;
        }


        public static void convertCsvtoHtml(string pathR,string pathW)
        {
            bool a = true;

            using (System.IO.StreamWriter writer = new System.IO.StreamWriter(pathW))
            {

                nagłówekPliku(writer);

                wieszNagłówkowy(writer, "HTML tabela");


                using (System.IO.StreamReader reader = new System.IO.StreamReader(pathR))
                {
                    string line;
                    while ((line = reader.ReadLine()) != null)
                    {
                        var text = line.Split(' ');

                        if (a)
                            a = wierszeDanych(writer, text);

                        else
                            stopkaPliku(writer, text);
                    }
                }

                koniecPliku(writer);
            }
        }
    }
}
