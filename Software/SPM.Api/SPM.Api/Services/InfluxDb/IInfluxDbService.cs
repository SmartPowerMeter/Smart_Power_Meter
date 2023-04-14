using SPM.Api.Core.Domain.Enums;
using SPM.Api.Services.InfluxDb.Models;

namespace SPM.Api.Services.InfluxDb
{
    public interface IInfluxDbService
    {
        Task<IEnumerable<MeasurementModel>> GetMeasurement(string measurementName, DateTimeOffset dateFrom, DateTimeOffset dateTo, string duration);
    }
}
