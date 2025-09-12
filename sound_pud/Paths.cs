using System.IO;
using System;

namespace sound_pud
{

    internal static class Paths
    {
        public static readonly string AudioPath;
        public static readonly string SettingsPath;

        static Paths()
        {
            // Папка, где находится exe
            string basePath = AppDomain.CurrentDomain.BaseDirectory;

            // Папка для аудио рядом с exe
            AudioPath = Path.Combine(basePath, "Sounds");
            if (!Directory.Exists(AudioPath))
                Directory.CreateDirectory(AudioPath);

            // Файл настроек
            SettingsPath = Path.Combine(basePath, "settings.json");
        }
    }
}

