using InfluxDB.Client;
using SPM.Api.Core.Extensions;
using SPM.Api.Services.InfluxDb.Models;

namespace SPM.Api.Services.InfluxDb
{
    public class InfluxDbService : IInfluxDbService
    {
        private readonly InfluxDbSettings _settings;

        public InfluxDbService(IConfiguration configuration)
        {
            _settings = configuration.GetSection("InfluxDb").Get<InfluxDbSettings>();
        }

        public async Task<IEnumerable<MeasurementModel>> GetMeasurement(string measurementName, DateTimeOffset dateFrom, DateTimeOffset dateTo, string duration)
        {
            var measurementReadings = await QueryAsync(async query =>
            {
                var flux = $"from(bucket:\"{_settings.Bucket}\") |> range(start: {dateFrom:s}Z, stop: {dateTo:s}Z) |> filter(fn: (r) => r[\"_measurement\"] == \"{measurementName}\") |> aggregateWindow(every: 1{duration}, fn: median)";
                var tables = await query.QueryAsync(flux, _settings.Organization);
                return tables.SelectMany(table =>
                    table.Records.Select(record =>
                        new MeasurementModel
                        {
                            Time = record.GetTime().ToString(),
                            Value = record.GetValue()?.ToString()?.ToDouble() ?? 0d
                        }));
            });

            return measurementReadings;
        }

        public async Task<IEnumerable<MeasurementModel>> GetRecentMeasurement(string measurementName, string time, int duration)
        {
            var measurementReadings = await QueryAsync(async query =>
            {
                var flux = $"from(bucket:\"{_settings.Bucket}\") |> range(start: -{time}) |> filter(fn: (r) => r[\"_measurement\"] == \"{measurementName}\") |> aggregateWindow(every: {duration}s, fn: median)";
                var tables = await query.QueryAsync(flux, _settings.Organization);
                return tables.SelectMany(table =>
                    table.Records.Select(record =>
                        new MeasurementModel
                        {
                            Time = record.GetTime().ToString(),
                            Value = record.GetValue()?.ToString()?.ToDouble() ?? 0d
                        }));
            });

            return measurementReadings;
        }

        private async Task<T> QueryAsync<T>(Func<QueryApi, Task<T>> action)
        {
            var options = new InfluxDBClientOptions(_settings.ClientUrl)
            {
                Token = _settings.Token,
                Timeout = TimeSpan.FromMinutes(1)
            };

            using var client = new InfluxDBClient(options);
            var query = client.GetQueryApi();
            return await action(query);
        }
    }
}
