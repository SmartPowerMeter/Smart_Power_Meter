using SPM.Api.Core.Domain.Enums;
using SPM.Api.Services.Measurements;
using SPM.Api.Services.InfluxDb.Models;
using SPM.Api.Services.Measurements.Models;

namespace SPM.Api.Services.Admin.Measurement
{
    public class AdminMeasurementService : IAdminMeasurementService
    {
        private readonly IMeasurementService _measurementService;

        public AdminMeasurementService(IMeasurementService measurementService)
        {
            _measurementService = measurementService;
        }

        public async Task<IEnumerable<MeasurementModel>> GetMeasurement(string customerId, MeasurementType measurement, TimeRange timeRange, AggregateDuration duration)
        {
            return await _measurementService.GetMeasurement(customerId, measurement, timeRange, duration);
        }

        public async Task<IEnumerable<MeasurementModel>> GetRecentMeasurement(string customerId, MeasurementType measurement, TimeType timeType, int time)
        {
            return await _measurementService.GetRecentMeasurement(customerId, measurement, timeType, time);
        }

        public async Task<GetEnergyConsumptionResponse> GetEnergyConsumption(string customerId, TimeRange timeRange)
        {
            return await _measurementService.GetEnergyConsumption(customerId, timeRange);
        }
    }
}
