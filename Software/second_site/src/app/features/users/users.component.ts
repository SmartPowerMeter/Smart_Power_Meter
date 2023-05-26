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

  public people: any[] = [
    {FullName: 'Shalva Butkhuzi', Email: 'shbutkhuzi@gmail.com'}
  ];

  constructor(_api: ApiService) { }

  ngOnInit(): void {
  }


  clicked() {
    if (!this.isToggleChecked) this.modalText = "Power will turn off.";
    else this.modalText = "Power will turn on.";
    
    this.isToggleChecked = !this.isToggleChecked;
    // this._api.SetRelayStatus(!this.isToggleChecked).subscribe((res)=>{
    //   this.isToggleChecked = !this.isToggleChecked;
    // })
  }

}
