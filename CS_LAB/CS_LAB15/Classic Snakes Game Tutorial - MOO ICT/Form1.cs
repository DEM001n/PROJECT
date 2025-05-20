using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace CS_LAB15
{
    public partial class Form1 : Form
    {
        private List<Circle> Snake = new List<Circle>();
        private Circle food = new Circle();

        int maxWidth;
        int maxHeight;

        Random rand = new Random();

        bool goLeft, goRight, goDown, goUp;

        int tickCount = 0;
        int maxSnakeSize; 

        public Form1()
        {
            InitializeComponent();

            picCanvas.Width = 80;
            picCanvas.Height = 80;

            new Settings();

            RestartGame();
        }

        private void KeyIsDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Left && Settings.directions != "right")
                Settings.directions = "left"; 
            if (e.KeyCode == Keys.Right && Settings.directions != "left")
                Settings.directions = "right"; 
            if (e.KeyCode == Keys.Up && Settings.directions != "down")
                Settings.directions = "up"; 
            if (e.KeyCode == Keys.Down && Settings.directions != "up")
                Settings.directions = "down"; 
        }

        private void KeyIsUp(object sender, KeyEventArgs e) { }
        
        private void GameTimerEvent(object sender, EventArgs e)
        {
            tickCount++;

            if (tickCount % 2 == 0)
                txtScore.Text = $"Time: {tickCount / 2} s";

            if (goLeft) Settings.directions = "left";
            if (goRight) Settings.directions = "right";
            if (goDown) Settings.directions = "down";
            if (goUp) Settings.directions = "up";

            for (int i = Snake.Count - 1; i >= 0; i--)
            {
                if (i == 0)
                {
                    switch (Settings.directions)
                    {
                        case "left": Snake[i].X--; break;
                        case "right": Snake[i].X++; break;
                        case "down": Snake[i].Y++; break;
                        case "up": Snake[i].Y--; break;
                    }

                    if (Snake[i].X < 0 || Snake[i].X > maxWidth-1 || Snake[i].Y < 0 || Snake[i].Y > maxHeight-1)
                    {
                        GameOver(false);
                    }


                    if (Snake[i].X == food.X && Snake[i].Y == food.Y) EatFood();
                    
                    for (int j = 1; j < Snake.Count; j++)
                    {
                        if (Snake[i].X == Snake[j].X && Snake[i].Y == Snake[j].Y)
                        {
                            GameOver(false);
                        }
                    }
                }
                else
                {
                    Snake[i].X = Snake[i - 1].X;
                    Snake[i].Y = Snake[i - 1].Y;
                }
            }

            picCanvas.Invalidate();
        }

        private void UpdatePictureBoxGraphics(object sender, PaintEventArgs e)
        {
            Graphics canvas = e.Graphics;
            Brush snakeColour;

            for (int i = 0; i < Snake.Count; i++)
            {
                snakeColour = (i == 0) ? Brushes.Black : Brushes.DarkGreen;

                canvas.FillEllipse(snakeColour, new Rectangle
                (
                    Snake[i].X * Settings.Width,
                    Snake[i].Y * Settings.Height,
                    Settings.Width, Settings.Height
                ));
            }

            canvas.FillEllipse(Brushes.DarkRed, new Rectangle
            (
                food.X * Settings.Width,
                food.Y * Settings.Height,
                Settings.Width, Settings.Height
            ));
        }

        private void RestartGame()
        {
            maxWidth = picCanvas.Width / Settings.Width ;
            maxHeight = picCanvas.Height / Settings.Height ;

            maxSnakeSize = (int)(picCanvas.Width / Settings.Width) * (picCanvas.Height / Settings.Height) - 2;

            progressBar1.Maximum = maxSnakeSize;

            Snake.Clear();

            progressBar1.Value = 0;

            int startX = maxWidth / 2;
            int startY = maxHeight / 2;

            Circle head = new Circle { X = startX, Y = startY };
            Snake.Add(head);

            Circle body = new Circle();
            Snake.Add(body);

            food = new Circle { X = rand.Next(0, maxWidth), Y = rand.Next(0, maxHeight) };

            gameTimer.Start();
        }

        private void EatFood()
        {
            if (progressBar1.Value + 1 >= maxSnakeSize)
            {
                progressBar1.Value = maxSnakeSize;
                GameOver(true);
            }
            else
            {
                progressBar1.Value += 1;
            }

            Circle body = new Circle
            {
                X = Snake[Snake.Count - 1].X,
                Y = Snake[Snake.Count - 1].Y
            };

            Snake.Add(body);

            int x = rand.Next(0, maxWidth);
            int y = rand.Next(0, maxHeight);

            int startX = x;
            int startY = y;

            bool isOccupied = true;

            while (isOccupied)
            {
                isOccupied = false;

                foreach (var part in Snake)
                {
                    if (part.X == x && part.Y == y)
                    {
                        isOccupied = true;
                        break;
                    }
                }

                if (isOccupied)
                {
                    x = (x + 1) % maxWidth;

                    if (x == startX)
                    {
                        y = (y + 1) % maxHeight;
                    }
                }
            }

            food = new Circle { X = x, Y = y };
        }

        private void GameOver(bool isVictory)
        {
            gameTimer.Stop();

            int timePlayed = tickCount / 2;

            if (isVictory)
            {
                MessageBox.Show($"Gratulacje! Wygrałeś! Czas gry: {timePlayed} sekund.", "Zwycięstwo!", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                MessageBox.Show($"Gra zakończona! Przegrałeś. Czas gry: {timePlayed} sekund.", "Koniec gry", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

        }
    }
}
