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
    }
}
