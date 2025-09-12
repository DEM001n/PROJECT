using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace sound_pud.DTO
{
    public class SoundConfig
    {
        public string FilePath { get; set; }
        public float Volume { get; set; }
        public float MinPlayInterval { get; set; }
        public bool Loop { get; set; }
    }
}
