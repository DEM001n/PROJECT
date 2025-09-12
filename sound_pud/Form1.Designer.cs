namespace sound_pud
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
            this.checkBox_all_bind = new System.Windows.Forms.CheckBox();
            this.buttonAddKey = new System.Windows.Forms.Button();
            this.buttonDelete = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // checkBox_all_bind
            // 
            this.checkBox_all_bind.AutoSize = true;
            this.checkBox_all_bind.Location = new System.Drawing.Point(58, 12);
            this.checkBox_all_bind.Name = "checkBox_all_bind";
            this.checkBox_all_bind.Size = new System.Drawing.Size(77, 20);
            this.checkBox_all_bind.TabIndex = 1;
            this.checkBox_all_bind.Text = "All_bind";
            this.checkBox_all_bind.UseVisualStyleBackColor = true;
            this.checkBox_all_bind.CheckedChanged += new System.EventHandler(this.checkBox_all_bind_CheckedChanged);
            // 
            // buttonAddKey
            // 
            this.buttonAddKey.Location = new System.Drawing.Point(58, 38);
            this.buttonAddKey.Name = "buttonAddKey";
            this.buttonAddKey.Size = new System.Drawing.Size(103, 23);
            this.buttonAddKey.TabIndex = 2;
            this.buttonAddKey.Text = "Add / Edit";
            this.buttonAddKey.UseVisualStyleBackColor = true;
            this.buttonAddKey.Click += new System.EventHandler(this.buttonAddKey_Click);
            // 
            // buttonDelete
            // 
            this.buttonDelete.Location = new System.Drawing.Point(58, 67);
            this.buttonDelete.Name = "buttonDelete";
            this.buttonDelete.Size = new System.Drawing.Size(103, 23);
            this.buttonDelete.TabIndex = 3;
            this.buttonDelete.Text = "Delete";
            this.buttonDelete.UseVisualStyleBackColor = true;
            this.buttonDelete.Click += new System.EventHandler(this.buttonDelete_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(228, 123);
            this.Controls.Add(this.buttonDelete);
            this.Controls.Add(this.buttonAddKey);
            this.Controls.Add(this.checkBox_all_bind);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.CheckBox checkBox_all_bind;
        private System.Windows.Forms.Button buttonAddKey;
        private System.Windows.Forms.Button buttonDelete;
    }
}

