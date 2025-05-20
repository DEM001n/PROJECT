using System;
using System.Windows.Forms;

namespace eve_calc_product
{
    public partial class Form1 : Form
    {
        private OreDatabase oreDatabase;

        public Form1()
        {
            InitializeComponent();
            oreDatabase = new OreDatabase();
        }
        
        private void btnGetMarketData_Click_1(object sender, EventArgs e)
        {
            string input = NameOre.Text.Trim();
            if (string.IsNullOrEmpty(input))
            {
                Info.Text = "Введите название или ID руды!";
                return;
            }

            Ore foundOre = oreDatabase.GetOreByNameOrId(input);

            if (foundOre == null)
            {
                Info.Text = "Руда не найдена!";
                return;
            }

            string result = $"Руда: {foundOre.OreName}\nСостав:\n";
            foreach (var mineral in foundOre.Minerals)
            {
                result += $"{mineral.MineralName}: {mineral.Quantity}\n";
            }

            Info.Text = result;
        }
        
    }
}
