using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace CS_LAB08
{
    public partial class Form1 : Form
    {
        private List<Person> people = new List<Person>(); // Original dataset

        public Form1()
        {
            InitializeComponent();

            comboBox1.DropDownStyle = ComboBoxStyle.DropDownList;
            comboBox1.Items.AddRange(new[] { "Male", "Female", "Other" });
            comboBox1.SelectedIndex = 0;
        }

        private void UpdateDataGridView()
        {
            // Create a list of DisplayPerson for binding to DataGridView
            var displayData = new List<DisplayPerson>();
            foreach (var person in people)
            {
                displayData.Add(new DisplayPerson(
                    person.FirstName,
                    person.LastName,
                    person.BirthDate.ToShortDateString(),
                    person.Gender
                ));
            }

            // Bind the list to DataGridView
            dataGridView1.DataSource = null;
            dataGridView1.DataSource = displayData;

            // Set column headers
            if (dataGridView1.Columns.Count > 0)
            {
                dataGridView1.Columns[0].HeaderText = "First Name";
                dataGridView1.Columns[1].HeaderText = "Last Name";
                dataGridView1.Columns[2].HeaderText = "Birth Date";
                dataGridView1.Columns[3].HeaderText = "Gender";
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(textBox1.Text) ||
                string.IsNullOrWhiteSpace(textBox2.Text) ||
                comboBox1.SelectedItem == null)
            {
                MessageBox.Show("Please fill in all fields!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            var person = new Person(
                textBox1.Text,
                textBox2.Text,
                dateTimePicker1.Value,
                comboBox1.SelectedItem.ToString()
            );

            people.Add(person); // Add to dataset
            UpdateDataGridView(); // Update the display list and refresh the DataGridView

            MessageBox.Show("Record added successfully!", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);

            textBox1.Clear();
            textBox2.Clear();
            comboBox1.SelectedIndex = -1;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (dataGridView1.CurrentRow == null)
            {
                MessageBox.Show("Please select a record to delete!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            int index = dataGridView1.CurrentRow.Index; // Get the selected index
            people.RemoveAt(index); // Remove from dataset
            UpdateDataGridView(); // Update the display list and refresh the DataGridView

            MessageBox.Show("Record deleted successfully!", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (dataGridView1.CurrentRow == null)
            {
                MessageBox.Show("Please select a record to edit!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            if (string.IsNullOrWhiteSpace(textBox1.Text) ||
                string.IsNullOrWhiteSpace(textBox2.Text) ||
                comboBox1.SelectedItem == null)
            {
                MessageBox.Show("Please fill in all fields!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            int index = dataGridView1.CurrentRow.Index; // Get the selected index
            people[index] = new Person(
                textBox1.Text,
                textBox2.Text,
                dateTimePicker1.Value,
                comboBox1.SelectedItem.ToString()
            );

            UpdateDataGridView(); // Update the display list and refresh the DataGridView

            MessageBox.Show("Record updated successfully!", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);

            textBox1.Clear();
            textBox2.Clear();
            comboBox1.SelectedIndex = -1;
        }

        private void dataGridView1_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex < 0) return;

            int index = e.RowIndex; // Ensure the index is valid
            var person = people[index]; // Get the selected person from the dataset

            // Populate fields with the selected record's data
            textBox1.Text = person.FirstName;
            textBox2.Text = person.LastName;
            dateTimePicker1.Value = person.BirthDate;
            comboBox1.SelectedItem = person.Gender;
        }

        private void textBox1_TextChanged(object sender, EventArgs e) { }
        private void textBox2_TextChanged(object sender, EventArgs e) { }
        private void dateTimePicker1_ValueChanged(object sender, EventArgs e) { }
        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e) { }
    }

    public struct Person
    {
        public string FirstName;
        public string LastName;
        public DateTime BirthDate;
        public string Gender;

        public Person(string firstName, string lastName, DateTime birthDate, string gender)
        {
            FirstName = firstName;
            LastName = lastName;
            BirthDate = birthDate;
            Gender = gender;
        }
    }

    public class DisplayPerson
    {
        public string FirstName { get; set; }
        public string LastName { get; set; }
        public string BirthDate { get; set; }
        public string Gender { get; set; }

        public DisplayPerson(string firstName, string lastName, string birthDate, string gender)
        {
            FirstName = firstName;
            LastName = lastName;
            BirthDate = birthDate;
            Gender = gender;
        }
    }
}
