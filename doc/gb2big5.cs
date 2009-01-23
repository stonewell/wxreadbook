using System;
using System.IO;
using System.Text;
using System.Collections.Generic;

public class MyClass
{
    public static void Main()
    {
        try
        {
            Dictionary<string, char> t = new Dictionary<string, char>();

            using (Stream fs = new FileStream("gb2big5.txt", FileMode.Open, FileAccess.Read))
            {
                using (StreamReader sr = new StreamReader(fs, Encoding.UTF8))
                {
                    using (Stream fs2 = new FileStream("gb2big5-char-array.txt", FileMode.Create, FileAccess.Write))
                    {
                        using (StreamWriter sw = new StreamWriter(fs2, Encoding.UTF8))
                        {
                            string line = sr.ReadLine();
                            bool simplifyFirst = true;

                            StreamWriter sw2 = new StreamWriter("gb2big5-compare.txt", false, Encoding.UTF8);

                            while (line != null)
                            {
                                if (string.Compare("zh2Hant", line) == 0)
                                {
                                    simplifyFirst = true;
                                }
                                else if (string.Compare("zh2Hans", line) == 0)
                                {
                                    simplifyFirst = false;
                                }
                                else if (line.Trim().Length > 0)
                                {
                                    string[] parts = line.Split(',');

                                    if (parts[0].Length == 1 || parts[0].Length == 3)
                                    {
                                        char ch1 = parts[0][parts[0].Length / 3];
                                        char ch2 = parts[1][parts[0].Length / 3];

                                        if (ch1 != ch2)
                                        {
                                            if (simplifyFirst)
                                            {
                                                if (!t.ContainsKey(string.Format("{0},{1}", ch1, ch2)))
                                                {
                                                    sw.WriteLine("{0},{1},",
                                                        Convert.ToUInt32(ch1),
                                                        Convert.ToUInt32(ch2));

                                                    sw2.WriteLine("{0},{1}", ch1, ch2);

                                                    t[string.Format("{0},{1}", ch1, ch2)] = ch2;

                                                    System.Console.WriteLine("{0},{1} {2},{3}",
                                                        ch1, ch2,
                                                        Convert.ToUInt32(ch1),
                                                        Convert.ToUInt32(ch2));
                                                }
                                            }
                                            else
                                            {
                                                if (!t.ContainsKey(string.Format("{0},{1}", ch2, ch1)))
                                                {
                                                    sw.WriteLine("{0},{1},",
                                                        Convert.ToUInt32(ch2),
                                                        Convert.ToUInt32(ch1));

                                                    sw2.WriteLine("{0},{1}", ch2, ch1);

                                                    t[string.Format("{0},{1}", ch2, ch1)] = ch1;
                                                    System.Console.WriteLine("{0},{1} {2},{3}",
                                                        ch2, ch1,
                                                        Convert.ToUInt32(ch2),
                                                        Convert.ToUInt32(ch1));
                                                }
                                            }
                                        }
                                    }
                                }

                                line = sr.ReadLine();
                            }

                            sw2.Flush();
                            sw2.Close();
                        }
                    }
                }
            }
        }
        catch (Exception ex)
        {
            System.Console.WriteLine(ex.Message);
            System.Console.WriteLine(ex.StackTrace);
        }

        RL();
    }

    #region Helper methods

    private static void WL(object text, params object[] args)
    {
        Console.WriteLine(text.ToString(), args);
    }

    private static void RL()
    {
        Console.ReadLine();
    }

    private static void Break()
    {
        System.Diagnostics.Debugger.Break();
    }

    #endregion
}