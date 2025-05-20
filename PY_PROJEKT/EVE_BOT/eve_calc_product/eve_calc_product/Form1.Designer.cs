namespace eve_calc_product
{
    partial class Form1
    {
        /// <summary>
        /// Wymagana zmienna projektanta.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Wyczyść wszystkie używane zasoby.
        /// </summary>
        /// <param name="disposing">prawda, jeżeli zarządzane zasoby powinny zostać zlikwidowane; Fałsz w przeciwnym wypadku.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Kod generowany przez Projektanta formularzy systemu Windows

        /// <summary>
        /// Metoda wymagana do obsługi projektanta — nie należy modyfikować
        /// jej zawartości w edytorze kodu.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnGetMarketData = new System.Windows.Forms.Button();
            this.Info = new System.Windows.Forms.Label();
            this.NameOre = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // btnGetMarketData
            // 
            this.btnGetMarketData.Location = new System.Drawing.Point(12, 7);
            this.btnGetMarketData.Name = "btnGetMarketData";
            this.btnGetMarketData.Size = new System.Drawing.Size(73, 22);
            this.btnGetMarketData.TabIndex = 0;
            this.btnGetMarketData.Text = "button1";
            this.btnGetMarketData.UseVisualStyleBackColor = true;
            this.btnGetMarketData.Click += new System.EventHandler(this.btnGetMarketData_Click_1);
            // 
            // Info
            // 
            this.Info.AutoSize = true;
            this.Info.Location = new System.Drawing.Point(12, 32);
            this.Info.Name = "Info";
            this.Info.Size = new System.Drawing.Size(44, 16);
            this.Info.TabIndex = 1;
            this.Info.Text = "label1";
            // 
            // NameOre
            // 
            this.NameOre.Location = new System.Drawing.Point(91, 7);
            this.NameOre.Name = "NameOre";
            this.NameOre.Size = new System.Drawing.Size(92, 22);
            this.NameOre.TabIndex = 2;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.NameOre);
            this.Controls.Add(this.Info);
            this.Controls.Add(this.btnGetMarketData);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnGetMarketData;
        private System.Windows.Forms.Label Info;
        private System.Windows.Forms.TextBox NameOre;
    }
}

