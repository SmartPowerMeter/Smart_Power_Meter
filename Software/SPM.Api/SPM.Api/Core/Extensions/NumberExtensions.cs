namespace SPM.Api.Core.Extensions
{
    public static class NumberExtensions
    {
        public static double ToDouble(this string str)
        {
            return Math.Round(double.TryParse(str, out double number) ? number : 0d, 2);
        }
    }
}
