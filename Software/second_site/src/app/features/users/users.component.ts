import { Component, OnInit } from '@angular/core';
import { ApiService } from 'src/app/services/api.service';

@Component({
  selector: 'app-users',
  templateUrl: './users.component.html',
  styleUrls: ['./users.component.scss']
})
export class UsersComponent implements OnInit {
  isToggleChecked: boolean = true;
  modalText: string = "Power will turn off.";

  public people: any;

  constructor(public _api: ApiService) { }

  ngOnInit(): void {

    this._api.GetAdminUsers().subscribe((res)=>{
      this.people = res;
      console.log(this.people)
    })

  }


  clicked(event: any) {
    if (!this.isToggleChecked) this.modalText = "Power will turn off.";
    else this.modalText = "Power will turn on.";
    
    this.isToggleChecked = !this.isToggleChecked;
    //  this._api.SetRelayStatus(!this.isToggleChecked).subscribe((res)=>{
    //    this.isToggleChecked = !this.isToggleChecked;
    //  })
    console.log(event.target.value)
  }

}
