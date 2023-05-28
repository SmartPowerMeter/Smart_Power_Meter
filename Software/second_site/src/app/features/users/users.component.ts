import { Component, OnInit } from "@angular/core";
import { ApiService } from "src/app/services/api.service";

@Component({
  selector: "app-users",
  templateUrl: "./users.component.html",
  styleUrls: ["./users.component.scss"],
})
export class UsersComponent implements OnInit {
  isToggleChecked: boolean = true;
  modalText: string = "Power will turn off.";
  clickedRowIndex: number;

  public people: any;

  constructor(public _api: ApiService) {}

  ngOnInit(): void {
    this._api.GetAdminUsers().subscribe((res) => {
      this.people = res;
      console.log(this.people);
    });
  }

  rowClicked(index: number) {
    this.clickedRowIndex = index;
  }

  clicked() {
    if (!this.isToggleChecked) this.modalText = "Power will turn off.";
    else this.modalText = "Power will turn on.";

    //  this._api.SetRelayStatus(!this.isToggleChecked).subscribe((res)=>{
    //    this.isToggleChecked = !this.isToggleChecked;
    //  })
    // if (this.clickedRowIndex !== undefined)
    //   console.log(this.people[this.clickedRowIndex].customerId)
    if (this.clickedRowIndex !== undefined) {
      this._api
        .SetAdminRelay(
          this.people[this.clickedRowIndex].customerId,
          !this.isToggleChecked
        )
        .subscribe((res) => {
          this.isToggleChecked = !this.isToggleChecked;
        });
    }
  }
}
