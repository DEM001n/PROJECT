using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;

namespace eve_calc_product
{
    public class Mineral
    {
        [JsonProperty("mineral_id")]
        public int MineralId { get; set; }

        [JsonProperty("mineral_name")]
        public string MineralName { get; set; }

        [JsonProperty("quantity")]
        public int Quantity { get; set; }
    }

    public class Ore
    {
        [JsonProperty("ore_id")]
        public int OreId { get; set; }

        [JsonProperty("ore_name")]
        public string OreName { get; set; }

        [JsonProperty("minerals")]
        public List<Mineral> Minerals { get; set; }
    }

    public class OreDatabase
    {
        private List<Ore> ores;

        public OreDatabase()
        {
            try
            {
                string filePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "ores_data.json");

                // Вывод пути в консоль для отладки
                Console.WriteLine("Загрузка файла: " + filePath);

                if (!File.Exists(filePath))
                {
                    throw new FileNotFoundException("Файл базы данных не найден!", filePath);
                }

                string json = File.ReadAllText(filePath);
                ores = JsonConvert.DeserializeObject<List<Ore>>(json) ?? new List<Ore>();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Ошибка загрузки базы данных: " + ex.Message);
                ores = new List<Ore>();
            }
        }

        public Ore GetOreByNameOrId(string input)
        {
            if (int.TryParse(input, out int oreId))
            {
                return ores.Find(ore => ore.OreId == oreId);
            }
            else
            {
                return ores.Find(ore => ore.OreName.Equals(input, StringComparison.OrdinalIgnoreCase));
            }
        }
    }
}
