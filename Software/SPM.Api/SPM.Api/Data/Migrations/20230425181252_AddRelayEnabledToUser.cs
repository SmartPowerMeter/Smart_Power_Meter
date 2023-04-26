using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace SPM.Api.Data.Migrations
{
    public partial class AddRelayEnabledToUser : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<bool>(
                name: "RelayEnabled",
                table: "User",
                type: "bit",
                nullable: false,
                defaultValue: false);
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "RelayEnabled",
                table: "User");
        }
    }
}
