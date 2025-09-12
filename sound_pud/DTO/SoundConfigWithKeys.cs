using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace sound_pud.DTO
{
    public class SoundConfigWithKeys
    {
        public SoundConfig SoundConfig { get; set; }
        public List<int> Keys { get; set; } = new List<int>();
    }
}
