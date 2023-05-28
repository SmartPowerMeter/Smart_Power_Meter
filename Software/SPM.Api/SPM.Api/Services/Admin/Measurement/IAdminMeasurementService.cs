using SPM.Api.Core.Domain.Enums;
using SPM.Api.Services.InfluxDb.Models;
using SPM.Api.Services.Measurements.Models;

namespace SPM.Api.Services.Admin.Measurement
{
    public interface IAdminMeasurementService
    {
        Task<IEnumerable<MeasurementModel>> GetMeasurement(string customerId, MeasurementType measurement, TimeRange timeRange, AggregateDuration duration);

        Task<IEnumerable<MeasurementModel>> GetRecentMeasurement(string customerId, MeasurementType measurement, TimeType timeType, int time);

        Task<GetEnergyConsumptionResponse> GetEnergyConsumption(string customerId, TimeRange timeRange);
    }
}
