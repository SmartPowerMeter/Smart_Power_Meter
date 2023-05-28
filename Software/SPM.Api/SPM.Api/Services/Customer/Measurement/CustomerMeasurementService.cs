using SPM.Api.Data;
using SPM.Api.Core.Domain.Enums;
using SPM.Api.Core.WorkContexts;
using SPM.Api.Services.Measurements;
using SPM.Api.Services.InfluxDb.Models;
using SPM.Api.Services.Measurements.Models;

namespace SPM.Api.Services.Customer.Measurement
{
    public class CustomerMeasurementService : ICustomerMeasurementService
    {
        private readonly IMeasurementService _measurementService;
        private readonly string _customerId;

        public CustomerMeasurementService(IMeasurementService measurementService, IWorkContext workContext, SPMDbContext dbContext)
        {
            _measurementService = measurementService;
            _customerId = dbContext.User.First(x => x.Id == workContext.CurrentUser.Id).CustomerId;
        }

        public async Task<IEnumerable<MeasurementModel>> GetMeasurement(MeasurementType measurement, TimeRange timeRange, AggregateDuration duration)
        {
            return await _measurementService.GetMeasurement(_customerId, measurement, timeRange, duration);
        }

        public async Task<IEnumerable<MeasurementModel>> GetRecentMeasurement(MeasurementType measurement, TimeType timeType, int time)
        {
            return await _measurementService.GetRecentMeasurement(_customerId, measurement, timeType, time);
        }

        public async Task<GetEnergyConsumptionResponse> GetEnergyConsumption(TimeRange timeRange)
        {
            return await _measurementService.GetEnergyConsumption(_customerId, timeRange);
        }
    }
}
