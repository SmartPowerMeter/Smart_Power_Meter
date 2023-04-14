using System.Reflection;
using SPM.Api.Core.Domain.Models;
using Microsoft.EntityFrameworkCore;

namespace SPM.Api.Data
{
    public class SPMDbContext : DbContext
    {
        public SPMDbContext(DbContextOptions<SPMDbContext> options) : base(options)
        {
        }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            modelBuilder.ApplyConfigurationsFromAssembly(Assembly.GetExecutingAssembly());
        }

        public DbSet<User> User { get; set; }
    }
}
