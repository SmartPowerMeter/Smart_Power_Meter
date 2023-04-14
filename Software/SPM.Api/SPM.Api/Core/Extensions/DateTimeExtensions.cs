using SPM.Api.Core.Domain.Enums;

namespace SPM.Api.Core.Extensions
{
    public static class DateTimeExtensions
    {
        public static DateTimeOffset StartTime(this DateTimeOffset currentDate, TimeRange timeRange)
        {
            switch (timeRange)
            {
                case TimeRange.Day: return currentDate.Date;
                case TimeRange.Week: return currentDate.FirstDayOfWeek();
                case TimeRange.Month: return currentDate.FirstDayOfMonth();
                case TimeRange.Year: return currentDate.FirstDayOfYear();
                default: return currentDate;
            }
        }

        private static DateTimeOffset FirstDayOfWeek(this DateTimeOffset currentDate)
        {
            var diff = currentDate.DayOfWeek - DayOfWeek.Monday;

            if (diff < 0)
                diff += 7;

            return currentDate.AddDays(-diff).Date;
        }

        private static DateTimeOffset FirstDayOfMonth(this DateTimeOffset currentDate)
        {
            return new DateTime(currentDate.Year, currentDate.Month, 1);
        }

        private static DateTimeOffset FirstDayOfYear(this DateTimeOffset currentDate)
        {
            return new DateTime(currentDate.Year, 1, 1);
        }
    }
}
