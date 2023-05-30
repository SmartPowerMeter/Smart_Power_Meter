import { ThrowStmt } from "@angular/compiler";
import { Component, OnInit } from "@angular/core";
import { Chart } from "chart.js";
import { ChartConfiguration } from "chart.js";
import { Subscription } from "rxjs";
import { ApiService } from "src/app/services/api.service";

export interface User {
  customerId: number;
  firstName: string;
  lastName: string;
  email: string;
  totalConsumedEnergy: number;
  totalCost: number;
}

@Component({
  selector: "app-dashboard",
  templateUrl: "./dashboard.component.html",
  styleUrls: ["./dashboard.component.scss"],
})
export class DashboardComponent implements OnInit {
  isToggleChecked: boolean;
  modalText: string;
  user: User;
  selectedOption: string = "Last 30 Minutes";
  lineChart1: any;
  lineChart2: any;
  lineChart3: any;
  lineChart4: any;
  lineChart5: any;
  lineChart6: any;
  lineChartCon1: number[];
  lineChartLabels1: string[];
  lineChartCon2: number[];
  lineChartLabels2: string[];
  lineChartCon3: number[];
  lineChartLabels3: string[];
  lineChartCon4: number[];
  lineChartLabels4: string[];
  lineChartCon5: number[];
  lineChartLabels5: string[];
  lineChartCon6: number[];
  lineChartLabels6: string[];
  serviceParams: any = {
    "Last 30 Minutes": { timeType: 1, time: 30 },
    "Last 1 Hour": { timeType: 2, time: 1 },
    "Last 3 Hours": { timeType: 2, time: 3 },
    "Last 6 Hours": { timeType: 2, time: 6 },
    "Last 12 Hours": { timeType: 2, time: 12 },
    "Last 24 Hours": { timeType: 2, time: 24 },
  };
  private subscr1: Subscription;
  private subscr2: Subscription;
  private subscr3: Subscription;
  private subscr4: Subscription;
  private subscr5: Subscription;
  private subscr6: Subscription;
  private subscr7: Subscription;
  private intervalId: any;

  constructor(public _api: ApiService) {
    this._api.GetUser().subscribe((res) => {
      this.user = res;
    });
  }

  ngOnInit(): void {
    this._api.GetRelayStatus().subscribe((res) => {
      if (res.adminRelayEnabled == true) { 
      this.isToggleChecked = res.customerRelayEnabled;
      if (this.isToggleChecked) this.modalText = "Power will turn off.";
      else this.modalText = "Power will turn on.";
      }
      else {
        this.modalText = "Your Power is Turned Off By Company.";
      }
    });

    this._api.RecentMeasurementPost(1, 1, 30).subscribe((res) => {
      this.lineChartCon1 = res.map((item: any) => item.value);
      this.lineChartLabels1 = res.map((item: any) => {
        const date = new Date(item.time);
        date.setHours(date.getHours() + 4);
        return date.toISOString().slice(11, 19);
      });
      this.createLineCharts();
    });

    this._api.RecentMeasurementPost(2, 1, 30).subscribe((res) => {
      this.lineChartCon2 = res.map((item: any) => item.value);
      this.lineChartLabels2 = res.map((item: any) => {
        const date = new Date(item.time);
        date.setHours(date.getHours() + 4);
        return date.toISOString().slice(11, 19);
      });
      this.lineChart2.data.datasets[0].data = this.lineChartCon2;
      this.lineChart2.data.labels = this.lineChartLabels2;
      this.lineChart2.update();
    });

    this._api.RecentMeasurementPost(3, 1, 30).subscribe((res) => {
      this.lineChartCon3 = res.map((item: any) => item.value);
      this.lineChartLabels3 = res.map((item: any) => {
        const date = new Date(item.time);
        date.setHours(date.getHours() + 4);
        return date.toISOString().slice(11, 19);
      });
      this.lineChart3.data.datasets[0].data = this.lineChartCon3;
      this.lineChart3.data.labels = this.lineChartLabels3;
      this.lineChart3.update();
    });

    this._api.RecentMeasurementPost(6, 1, 30).subscribe((res) => {
      this.lineChartCon4 = res.map((item: any) => item.value);
      this.lineChartLabels4 = res.map((item: any) => {
        const date = new Date(item.time);
        date.setHours(date.getHours() + 4);
        return date.toISOString().slice(11, 19);
      });
      this.lineChart4.data.datasets[0].data = this.lineChartCon4;
      this.lineChart4.data.labels = this.lineChartLabels4;
      this.lineChart4.update();
    });

    this._api.RecentMeasurementPost(5, 1, 30).subscribe((res) => {
      this.lineChartCon5 = res.map((item: any) => item.value);
      this.lineChartLabels5 = res.map((item: any) => {
        const date = new Date(item.time);
        date.setHours(date.getHours() + 4);
        return date.toISOString().slice(11, 19);
      });
      this.lineChart5.data.datasets[0].data = this.lineChartCon5;
      this.lineChart5.data.labels = this.lineChartLabels5;
      this.lineChart5.update();
    });

    this._api.RecentMeasurementPost(4, 1, 30).subscribe((res) => {
      this.lineChartCon6 = res.map((item: any) => item.value);
      this.lineChartLabels6 = res.map((item: any) => {
        const date = new Date(item.time);
        date.setHours(date.getHours() + 4);
        return date.toISOString().slice(11, 19);
      });
      this.lineChart6.data.datasets[0].data = this.lineChartCon6;
      this.lineChart6.data.labels = this.lineChartLabels6;
      this.lineChart6.update();
    });

    this.intervalId = setInterval(() => {
      const params = this.serviceParams[this.selectedOption];
      this.send(
        this.serviceParams[this.selectedOption].timeType,
        this.serviceParams[this.selectedOption].time
      );
    }, 1000);

    this._api.GetEnergyConsumption(1).subscribe((res) => {
      const obj: any = res;
      this.user.totalConsumedEnergy = obj.totalConsumedEnergy;
      this.user.totalCost = obj.totalCost;
    });
  }

  clicked() {
    this._api.GetRelayStatus().subscribe((res) => {
      if (res.adminRelayEnabled == true) {
        this.isToggleChecked = res.customerRelayEnabled;
        if (this.isToggleChecked) this.modalText = "Power will turn off.";
        else this.modalText = "Power will turn on.";
        this._api.SetRelayStatus(!this.isToggleChecked).subscribe((res) => {
          this.isToggleChecked = !this.isToggleChecked;
        });
      }
      else{
        this.modalText = "Your Power is Turned Off By Company.";
      }
    });
  }

  createLineCharts() {
    this.lineChart1 = this.createLineChart(
      "MyLineChart1",
      this.lineChartLabels1,
      this.lineChartCon1
    );
    this.lineChart2 = this.createLineChart(
      "MyLineChart2",
      this.lineChartLabels2,
      this.lineChartCon2
    );
    this.lineChart3 = this.createLineChart(
      "MyLineChart3",
      this.lineChartLabels3,
      this.lineChartCon3
    );
    this.lineChart4 = this.createLineChart(
      "MyLineChart4",
      this.lineChartLabels4,
      this.lineChartCon4
    );
    this.lineChart5 = this.createLineChart(
      "MyLineChart5",
      this.lineChartLabels5,
      this.lineChartCon5
    );
    this.lineChart6 = this.createLineChart(
      "MyLineChart6",
      this.lineChartLabels6,
      this.lineChartCon6
    );
  }

  createLineChart(chartName: any, labels: any, chartData: any) {
    return new Chart(chartName, {
      type: "line", //this denotes tha type of chart

      data: {
        // values on X-Axis
        labels: labels,
        datasets: [
          {
            label: "Consumed",
            data: chartData,
            backgroundColor: "limegreen",
            borderColor: "#7ADC24",
            tension: 0.4,
          },
        ],
      },
      options: {
        aspectRatio: 2.5,
      },
    });
  }

  selectOption(option: string) {
    this.selectedOption = option;
  }

  send(timeType: number, time: number) {
    this.subscr1 = this._api
      .RecentMeasurementPost(1, timeType, time)
      .subscribe((res) => {
        this.lineChartCon1 = res.map((item: any) => item.value);
        this.lineChartLabels1 = res.map((item: any) => {
          const date = new Date(item.time);
          date.setHours(date.getHours() + 4);
          return date.toISOString().slice(11, 19);
        });
        this.lineChart1.data.datasets[0].data = this.lineChartCon1;
        this.lineChart1.data.labels = this.lineChartLabels1;
        this.lineChart1.update();
      });

    this.subscr2 = this._api
      .RecentMeasurementPost(2, timeType, time)
      .subscribe((res) => {
        this.lineChartCon2 = res.map((item: any) => item.value);
        this.lineChartLabels2 = res.map((item: any) => {
          const date = new Date(item.time);
          date.setHours(date.getHours() + 4);
          return date.toISOString().slice(11, 19);
        });
        this.lineChart2.data.datasets[0].data = this.lineChartCon2;
        this.lineChart2.data.labels = this.lineChartLabels2;
        this.lineChart2.update();
      });

    this.subscr3 = this._api
      .RecentMeasurementPost(3, timeType, time)
      .subscribe((res) => {
        this.lineChartCon3 = res.map((item: any) => item.value);
        this.lineChartLabels3 = res.map((item: any) => {
          const date = new Date(item.time);
          date.setHours(date.getHours() + 4);
          return date.toISOString().slice(11, 19);
        });
        this.lineChart3.data.datasets[0].data = this.lineChartCon3;
        this.lineChart3.data.labels = this.lineChartLabels3;
        this.lineChart3.update();
      });

    this.subscr4 = this._api
      .RecentMeasurementPost(6, timeType, time)
      .subscribe((res) => {
        this.lineChartCon4 = res.map((item: any) => item.value);
        this.lineChartLabels4 = res.map((item: any) => {
          const date = new Date(item.time);
          date.setHours(date.getHours() + 4);
          return date.toISOString().slice(11, 19);
        });
        this.lineChart4.data.datasets[0].data = this.lineChartCon4;
        this.lineChart4.data.labels = this.lineChartLabels4;
        this.lineChart4.update();
      });

    this.subscr5 = this._api
      .RecentMeasurementPost(5, timeType, time)
      .subscribe((res) => {
        this.lineChartCon5 = res.map((item: any) => item.value);
        this.lineChartLabels5 = res.map((item: any) => {
          const date = new Date(item.time);
          date.setHours(date.getHours() + 4);
          return date.toISOString().slice(11, 19);
        });
        this.lineChart5.data.datasets[0].data = this.lineChartCon5;
        this.lineChart5.data.labels = this.lineChartLabels5;
        this.lineChart5.update();
      });

    this.subscr6 = this._api
      .RecentMeasurementPost(4, timeType, time)
      .subscribe((res) => {
        this.lineChartCon6 = res.map((item: any) => item.value);
        this.lineChartLabels6 = res.map((item: any) => {
          const date = new Date(item.time);
          date.setHours(date.getHours() + 4);
          return date.toISOString().slice(11, 19);
        });
        this.lineChart6.data.datasets[0].data = this.lineChartCon6;
        this.lineChart6.data.labels = this.lineChartLabels6;
        this.lineChart6.update();
      });
  }

  ngOnDestroy(): void {
    clearInterval(this.intervalId);
    this.subscr1.unsubscribe();
    this.subscr2.unsubscribe();
    this.subscr3.unsubscribe();
    this.subscr4.unsubscribe();
    this.subscr5.unsubscribe();
    this.subscr6.unsubscribe();

    // this._api.SetRelayStatus(this.isToggleChecked).subscribe((res)=>{
    //   console.log(res)
    // })
  }
}

// MeasurementType: any = {
//   Voltage: 1,
//   Current: 2,
//   Power: 3,
//   Frequency: 4,
//   "Power Factor": 5,
//   Energy: 6,
// };
