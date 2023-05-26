import { Component, HostListener, OnInit } from '@angular/core';
import { LayoutService } from '../services/layout.service';
import { CardService } from '../../features/accounts/card.service';
import { Router } from '@angular/router';
import { ApiService } from 'src/app/services/api.service';

@Component({
  selector: 'app-sidebar',
  templateUrl: './sidebar.component.html',
  styleUrls: ['./sidebar.component.scss'],
})
export class SidebarComponent implements OnInit {
  user: any;
  isCollapsed: boolean;
  screenSize: number;
  cardsArray: any = [];
  constructor(
    private ls: LayoutService,
    private CardService: CardService,
    private router: Router,
    private _api: ApiService,
  ) {}

  ngOnInit(): void {
    this.ls.sidebarStatus$.subscribe(value => (this.isCollapsed = value));

    this.CardService.getCards().subscribe(data => {
      this.cardsArray = data;
    });
    this.screenSize = window.innerWidth;
    if (this.screenSize < 576) {
      this.ls.updateStatus(true);
    }

    this._api.GetUser().subscribe((res)=>{
      this.user = res;
    });
  }

  @HostListener('window:resize', ['$event'])
  onWindowResize() {
    this.screenSize = window.innerWidth;
    if (this.screenSize < 576) {
      if (!this.isCollapsed) {
        this.ls.updateStatus(true);
      }
    } else {
      if (this.isCollapsed) {
        this.ls.updateStatus(false);
      }
    }
  }
}
