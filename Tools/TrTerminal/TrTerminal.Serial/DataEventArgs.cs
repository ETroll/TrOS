using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrTerminal.Serial
{
    public class DataEventArgs : EventArgs
    {
        public byte[] Data { get; set; }
    }
}
