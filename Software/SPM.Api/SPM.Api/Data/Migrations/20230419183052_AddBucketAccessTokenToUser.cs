using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace SPM.Api.Data.Migrations
{
    public partial class AddBucketAccessTokenToUser : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<string>(
                name: "BucketAccessToken",
                table: "User",
                type: "nvarchar(200)",
                maxLength: 200,
                nullable: false,
                defaultValue: "");
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "BucketAccessToken",
                table: "User");
        }
    }
}
