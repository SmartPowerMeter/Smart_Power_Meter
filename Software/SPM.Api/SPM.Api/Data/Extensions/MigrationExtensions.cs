using Microsoft.EntityFrameworkCore;

namespace SPM.Api.Data.Extensions
{
    public static class MigrationExtensions
    {
        public static IServiceProvider MigrateDb(this IServiceProvider serviceProvider)
        {
            using (var serviceScope = serviceProvider.GetRequiredService<IServiceScopeFactory>().CreateScope())
            {
                var context = serviceScope.ServiceProvider.GetService<SPMDbContext>();
                context.Database.Migrate();
            }

            return serviceProvider;
        }
    }
}
