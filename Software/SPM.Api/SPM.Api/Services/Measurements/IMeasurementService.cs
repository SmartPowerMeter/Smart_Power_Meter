using SPM.Api.Core.Domain.Enums;
using SPM.Api.Services.InfluxDb.Models;
using SPM.Api.Services.Measurements.Models;

namespace SPM.Api.Services.Measurements
{
    public interface IMeasurementService
    {
        Task<IEnumerable<MeasurementModel>> GetMeasurement(MeasurementType measurement, TimeRange timeRange, AggregateDuration duration);

        Task<IEnumerable<MeasurementModel>> GetRecentMeasurement(MeasurementType measurement, TimeType timeType, int time);

        Task<GetMonthlyEnergyConsumptionResponse> GetMonthlyEnergyConsumption();
    }
}
