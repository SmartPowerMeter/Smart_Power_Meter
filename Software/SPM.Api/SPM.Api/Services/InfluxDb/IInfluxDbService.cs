using SPM.Api.Services.InfluxDb.Models;

namespace SPM.Api.Services.InfluxDb
{
    public interface IInfluxDbService
    {
        Task<IEnumerable<MeasurementModel>> GetMeasurement(string measurementName, DateTimeOffset dateFrom, DateTimeOffset dateTo, string duration);

        Task<IEnumerable<MeasurementModel>> GetRecentMeasurement(string measurementName, string time, int duration);
    }
}
