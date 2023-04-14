namespace SPM.Api.Core.Extensions
{
    public static class NumberExtensions
    {
        public static double ToDouble(this string str)
        {
            return double.TryParse(str, out double number) ? number : 0d;
        }
    }
}
