using SPM.Api.Core.Extensions;
using SPM.Api.Core.Domain.Enums;
using SPM.Api.Services.InfluxDb;
using SPM.Api.Services.InfluxDb.Models;

namespace SPM.Api.Services.Measurements
{
    public class MeasurementService : IMeasurementService
    {
        private readonly IInfluxDbService _influxDbService;

        public MeasurementService(IInfluxDbService influxDbService)
        {
            _influxDbService = influxDbService;
        }

        public async Task<IEnumerable<MeasurementModel>> GetMeasurement(MeasurementType measurement, TimeRange timeRange, AggregateDuration duration)
        {
            var currentDate = DateTimeOffset.Now;
            var data = await _influxDbService.GetMeasurement(measurement.ToString(), currentDate.StartTime(timeRange), currentDate, duration.ToStringDuration());

            return data;
        }

        public async Task<IEnumerable<MeasurementModel>> GetRecentMeasurement(MeasurementType measurement, TimeType timeType, int time)
        {
            var data = await _influxDbService.GetRecentMeasurement(measurement.ToString(), GetTime(timeType, time), GetDuration(timeType, time));

            return data;
        }

        private string GetTime(TimeType timeType, int time)
        {
            return timeType switch
            {
                TimeType.Minute => $"{time}m",
                TimeType.Hour => $"{time}h",
                _ => "",
            };
        }

        private int GetDuration(TimeType timeType, int time)
        {
            var windowSize = 30;

            switch (timeType)
            {
                case TimeType.Minute:
                    return time * 60 / windowSize;
                case TimeType.Hour:
                    return time * 3600 / windowSize;
                default:
                    return 0;
            }
        }
    }
}
