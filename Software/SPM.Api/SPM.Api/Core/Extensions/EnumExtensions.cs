using SPM.Api.Core.Domain.Enums;

namespace SPM.Api.Core.Extensions
{
    public static class EnumExtensions
    {
        public static string ToStringDuration(this AggregateDuration duration)
        {
            switch (duration)
            {
                case AggregateDuration.Hour: return "h";
                case AggregateDuration.Day: return "d";
                case AggregateDuration.Month: return "mo";
                case AggregateDuration.Year: return "y";
            }
            
            return string.Empty;
        }
    }
}
