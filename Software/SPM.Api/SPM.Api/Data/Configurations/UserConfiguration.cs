using SPM.Api.Core.Constants;
using SPM.Api.Core.Domain.Models;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata.Builders;

namespace SPM.Api.Data.Configurations
{
    public class UserConfiguration : IEntityTypeConfiguration<User>
    {
        public void Configure(EntityTypeBuilder<User> builder)
        {
            builder.HasKey(p => p.Id);
            builder.Property(x => x.FirstName).IsRequired().HasMaxLength(FieldLengthConstants.Small);
            builder.Property(x => x.LastName).IsRequired().HasMaxLength(FieldLengthConstants.Small);
            builder.Property(x => x.Password).IsRequired().HasMaxLength(FieldLengthConstants.Small);
            builder.Property(x => x.Email).IsRequired().HasMaxLength(FieldLengthConstants.ExtraSmall);
            builder.Property(x => x.CustomerId).IsRequired().HasMaxLength(FieldLengthConstants.ExtraSmall);
            builder.Property(x => x.BucketAccessToken).IsRequired().HasMaxLength(FieldLengthConstants.Medium);
        }
    }
}
