using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace SPM.Api.Data.Migrations
{
    public partial class AddAdminRelayEnabledToUser : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.RenameColumn(
                name: "RelayEnabled",
                table: "User",
                newName: "CustomerRelayEnabled");

            migrationBuilder.AddColumn<bool>(
                name: "AdminRelayEnabled",
                table: "User",
                type: "bit",
                nullable: false,
                defaultValue: false);
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "AdminRelayEnabled",
                table: "User");

            migrationBuilder.RenameColumn(
                name: "CustomerRelayEnabled",
                table: "User",
                newName: "RelayEnabled");
        }
    }
}
