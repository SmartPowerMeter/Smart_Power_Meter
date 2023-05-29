import { Component, OnInit } from "@angular/core";
import { ApiService } from "src/app/services/api.service";

@Component({
  selector: "app-users",
  templateUrl: "./users.component.html",
  styleUrls: ["./users.component.scss"],
})
export class UsersComponent implements OnInit {
  //isToggleChecked: boolean = true;
  modalText: string ;
  clickedRowIndex: number;

  public people: any;

  constructor(public _api: ApiService) {}

  ngOnInit(): void {

    this._api.GetAdminUsers().subscribe((res) => {
      this.people = res;
      for (let i = 0; i < this.people.length; i++) {
        if(this.people[i].customerRelayEnabled)
          this.modalText = "Power will turn off.";
        else this.modalText = "Power will turn on.";
      }
    });

  }

  rowClicked(index: number) {
    this.clickedRowIndex = index;
  }

  clicked() {
    //  this._api.SetRelayStatus(!this.isToggleChecked).subscribe((res)=>{
    //    this.isToggleChecked = !this.isToggleChecked;
    //  })
    // if (this.clickedRowIndex !== undefined)
    //   console.log(this.people[this.clickedRowIndex].customerId)
    if (this.clickedRowIndex !== undefined) {
      if (this.people[this.clickedRowIndex].customerRelayEnabled == false)
        this.modalText = "Power will turn off.";
      else this.modalText = "Power will turn on.";

      this._api
        .SetAdminRelay(
          this.people[this.clickedRowIndex].customerId,
          !this.people[this.clickedRowIndex].customerRelayEnabled
        )
        .subscribe((res) => {
          this._api.GetAdminUsers().subscribe((res) => {
            this.people = [];
            this.people = res;
          });
        });
    }
  }
}
