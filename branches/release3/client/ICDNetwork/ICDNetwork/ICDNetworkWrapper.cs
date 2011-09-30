using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ICDNetwork
{
    public class ICDNetworkWrapper
    {
        [DllImport("ICDNetworkCPP.dll")]
        //public static extern int puts(
        //    [MarshalAs(UnmanagedType.LPStr)]
        //string m);
    }
}
