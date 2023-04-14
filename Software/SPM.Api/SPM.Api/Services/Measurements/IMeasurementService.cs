using SPM.Api.Core.Domain.Enums;
using SPM.Api.Services.InfluxDb.Models;

namespace SPM.Api.Services.Measurements
{
    public interface IMeasurementService
    {
        Task<IEnumerable<MeasurementModel>> GetMeasurement(MeasurementType measurement, TimeRange timeRange, AggregateDuration duration);
    }
}
