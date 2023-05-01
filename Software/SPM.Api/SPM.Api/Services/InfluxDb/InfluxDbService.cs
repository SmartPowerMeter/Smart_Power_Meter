using SPM.Api.Data;
using InfluxDB.Client;
using SPM.Api.Core.Extensions;
using SPM.Api.Core.WorkContexts;
using InfluxDB.Client.Api.Domain;
using Microsoft.EntityFrameworkCore;
using SPM.Api.Services.InfluxDb.Models;

namespace SPM.Api.Services.InfluxDb
{
    public class InfluxDbService : IInfluxDbService
    {
        private readonly InfluxDbSettings _settings;
        private readonly IWorkContext _workContext;
        private readonly SPMDbContext _dbContext;

        public InfluxDbService(IConfiguration configuration, IWorkContext workContext, SPMDbContext dbContext)
        {
            _workContext = workContext;
            _dbContext = dbContext;
            _settings = configuration.GetSection("InfluxDb").Get<InfluxDbSettings>();
        }

        public async Task<IEnumerable<MeasurementModel>> GetMeasurement(string measurementName, DateTimeOffset dateFrom, DateTimeOffset dateTo, string duration)
        {
            var user = await _dbContext.User.AsNoTracking().FirstAsync(x => x.Id == _workContext.CurrentUser.Id);

            var measurementReadings = await QueryAsync(async query =>
            {
                var flux = $"from(bucket:\"{user.GetBucketName()}\") |> range(start: {dateFrom:s}Z, stop: {dateTo:s}Z) |> filter(fn: (r) => r[\"_measurement\"] == \"{measurementName}\") |> aggregateWindow(every: 1{duration}, fn: mean)";
                var tables = await query.QueryAsync(flux, _settings.Organization);
                return tables.SelectMany(table =>
                    table.Records.Select(record =>
                        new MeasurementModel
                        {
                            Time = record.GetTime().ToString(),
                            Value = record.GetValue()?.ToString()?.ToDouble() ?? 0d
                        }));
            }, user.BucketAccessToken);

            return measurementReadings;
        }

        public async Task<IEnumerable<MeasurementModel>> GetRecentMeasurement(string measurementName, string time, int duration)
        {
            var user = await _dbContext.User.AsNoTracking().FirstAsync(x => x.Id == _workContext.CurrentUser.Id);

            var measurementReadings = await QueryAsync(async query =>
            {
                var flux = $"from(bucket:\"{user.GetBucketName()}\") |> range(start: -{time}) |> filter(fn: (r) => r[\"_measurement\"] == \"{measurementName}\") |> aggregateWindow(every: {duration}s, fn: mean)";
                var tables = await query.QueryAsync(flux, _settings.Organization);
                return tables.SelectMany(table =>
                    table.Records.Select(record =>
                        new MeasurementModel
                        {
                            Time = record.GetTime().ToString(),
                            Value = record.GetValue()?.ToString()?.ToDouble() ?? 0d
                        }));
            }, user.BucketAccessToken);
            return measurementReadings;
        }

        public async Task<string> CreateBucket(string customerId)
        {
            using var client = new InfluxDBClient(_settings.ClientUrl, _settings.Token);

            var orgId = (await client.GetOrganizationsApi().FindOrganizationsAsync(org: _settings.Organization)).First().Id;

            var bucket = await client.GetBucketsApi().CreateBucketAsync($"{customerId}_bucket", null, orgId);

            var resource = new PermissionResource(PermissionResource.TypeBuckets, bucket.Id, null, orgId);
            var read = new Permission(Permission.ActionEnum.Read, resource);
            var write = new Permission(Permission.ActionEnum.Write, resource);

            var authorization = await client.GetAuthorizationsApi()
                .CreateAuthorizationAsync(orgId, new List<Permission> { read, write });

            return authorization.Token;
        }

        private async Task<T> QueryAsync<T>(Func<QueryApi, Task<T>> action, string token)
        {
            var options = new InfluxDBClientOptions(_settings.ClientUrl)
            {
                Token = token,
                Timeout = TimeSpan.FromMinutes(1)
            };

            using var client = new InfluxDBClient(options);
            var query = client.GetQueryApi();
            return await action(query);
        }
    }
}
