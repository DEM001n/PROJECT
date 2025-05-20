using System;
using System.Windows.Forms;

namespace CS_LAB06_TRUE
{
    public partial class Form1 : Form
    {
        private Calculate calculator = new Calculate();

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            label1.Text = "0";
        }

        //======================================================================
        private void button1_Click(object sender, EventArgs e) => InputNumber(1);
        private void button2_Click(object sender, EventArgs e) => InputNumber(2);
        private void button3_Click(object sender, EventArgs e) => InputNumber(3);
        private void button4_Click(object sender, EventArgs e) => InputNumber(4);
        private void button5_Click(object sender, EventArgs e) => InputNumber(5);
        private void button6_Click(object sender, EventArgs e) => InputNumber(6);
        private void button7_Click(object sender, EventArgs e) => InputNumber(7);
        private void button8_Click(object sender, EventArgs e) => InputNumber(8);
        private void button9_Click(object sender, EventArgs e) => InputNumber(9);
        private void button10_Click(object sender, EventArgs e) => InputNumber(0);

        // =========================================================================
        private void button11_Click(object sender, EventArgs e) => SetOperation(1); // '-'
        private void button12_Click(object sender, EventArgs e) => SetOperation(2); // '+'
        private void button13_Click(object sender, EventArgs e) => SetOperation(3); // '*'
        private void button14_Click(object sender, EventArgs e) => SetOperation(4); // '/'

        // =========================================================================
        private void button15_Click(object sender, EventArgs e)
        {
            label1.Text = calculator.MakeCalculation();
        }

        private void button16_Click(object sender, EventArgs e)
        {
            calculator.CE_clean();
            label1.Text = "0";
        }

        private void button17_Click(object sender, EventArgs e)
        {
            calculator.ClearLastEntry();
            label1.Text = calculator.DisplayText();
        }

        private void button18_Click(object sender, EventArgs e)
        {
            calculator.DelNum();
            label1.Text = calculator.DisplayText();
        }

        private void button19_Click(object sender, EventArgs e)
        {
            calculator.ToggleSign();
            label1.Text = calculator.DisplayText();
        }

        private void InputNumber(int number)
        {
            calculator.MakeNum(number);
            label1.Text = calculator.DisplayText();
        }

        private void SetOperation(int operation)
        {
            calculator.SetOperation(operation);
            label1.Text = calculator.DisplayText();
        }
    }

    public class Calculate
    {
        private int num1 = 0;
        private int num2 = 0;
        private int operation = 0;// (1 = '-', 2 = '+', 3 = '*', 4 = '/')
        private bool isSecondNumber = false;

        public void MakeNum(int number)
        {
            if (isSecondNumber)
            {
                num2 = (num2 != 0 ? num2 * 10 : 0) + number;
            }
            else
            {
                num1 = (num1 != 0 ? num1 * 10 : 0) + number;
            }
        }

        public void SetOperation(int operation)
        {
            this.operation = operation;
            isSecondNumber = true;
        }

        public string MakeCalculation()
        {
            int result = 0;

            switch (operation)
            {
                case 1:
                    result = num1 - num2;
                    break;
                case 2:
                    result = num1 + num2;
                    break;
                case 3:
                    result = num1 * num2;
                    break;
                case 4:
                    if (num2 == 0)
                        MessageBox.Show("Error: Division by zero is not possible", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    else
                        result = num1 / num2;
                    break;
                default:
                    result = num1;
                    break;
            }

            num1 = result;
            num2 = 0;
            isSecondNumber = false;
            operation = 0;

            return result.ToString();
        }


        public void DelNum()
        {
            if (isSecondNumber)
            {
                if (num2 == 0)
                {
                    operation = 0;
                    isSecondNumber = false;
                }
                else
                {
                    num2 /= 10;
                }
            }
            else
            {
                num1 /= 10;
            }
        }

        public void CE_clean()
        {
            num1 = 0;
            num2 = 0;
            operation = 0;
            isSecondNumber = false;
        }

        public void ClearLastEntry()
        {
            if (isSecondNumber)
            {
                operation = 0;
                isSecondNumber = false;
                num2 = 0;
            }
            else
            {
                CE_clean();
            }
        }

        public void ToggleSign()
        {
            if (isSecondNumber)
            {
                num2 = -num2;
            }
            else
            {
                num1 = -num1;
            }
        }

        public string DisplayText()
        {
            return isSecondNumber ? $"{num1} {GetOperationSymbol()} {num2}" : $"{num1}";
        }

        private string GetOperationSymbol()
        {
            switch (operation)
            {
                case 1:
                    return "-";
                case 2:
                    return "+";
                case 3:
                    return "*";
                case 4:
                    return "/";
                default:
                    return "";
            }
        }
    }
}
