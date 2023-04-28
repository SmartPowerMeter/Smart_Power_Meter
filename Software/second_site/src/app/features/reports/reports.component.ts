import { HttpClient } from "@angular/common/http";
import { interval } from "rxjs";
import { Component, Input, OnChanges, OnInit, ViewChild } from "@angular/core";
import Chart from "chart.js/auto";

import { LayoutService } from "src/app/layout/services/layout.service";
import { PaymentsService } from "../payments/payments.service";
import { Router } from "@angular/router";
import { Subscription } from "rxjs";
import { UserService } from "src/app/services/user.service";
import { ApiService } from "../../services/api.service";

@Component({
  selector: "app-reports",
  templateUrl: "./reports.component.html",
  styleUrls: ["./reports.component.scss"],
})
export class ReportsComponent implements OnInit, OnChanges {
  paymentsArray: any;
  lineChart: any;
  barChart: any;
  pieChart: any;
  //ამას დინამიური განახლებისთვის ვაპირებდი მაგრამ არ ვარგა
  // subscription: any = interval(1000).subscribe(() => {
  //   this.barChart.destroy();
  //   this._api.GetUserStats(2).subscribe((res) => {
  //     this.barChartAvg = res.avgTenDays;
  //     this.barChartCon = res.conTenDays;
  //     this.createBarChart();
  //     this.createLineChart();

  //   });
  // });

  barChartLabels: String[];
  barChartAvg: Number[];
  barChartCon: Number[];
  lineChartLabels: String[];
  lineChartAvg: Number[];
  lineChartCon: Number[];
  TimeRange: any = { Day: 1, Week: 2, Month: 3, Year: 4 };
  MeasurementType: any = {
    Voltage: 1,
    Current: 2,
    Power: 3,
    Frequency: 4,
    "Power Factor": 5,
    Energy: 6,
  };
  selectedOption: string = "Energy";
  selectedOption1: string = "Week";

  constructor(
    private paymentsService: PaymentsService,
    public _api: ApiService
  ) {
    const today = new Date();
    const previousDates = [];
    //generating previous 10 days
    for (let i = 9; i >= 0; i--) {
      const previousDate = new Date(today);
      previousDate.setDate(today.getDate() - i);
      previousDates.push(previousDate.toISOString().slice(0, 10));
    }

    this.barChartLabels = previousDates;

    this.lineChartLabels = [
      `0`,
      "2022-05-11",
      "2022-05-12",
      "2022-05-13",
      "2022-05-14",
      "2022-05-15",
      "2022-05-16",
      "2022-05-17",
    ];
  }

  ngOnInit(): void {
    // this.paymentsService.getData().subscribe((payments) => {
    //   this.paymentsArray = this.paymentsService.data;
    // });
    this._api.MeasurementPost(6, 2, 2).subscribe((res) => {
      this.barChartCon = [];
      this.barChartCon = res.map((item: any) => item.value);
      this.barChartLabels = [];
      this.barChartLabels = res.map((item: any) => item.time.slice(0, 10));;
      // this.barChartAvg = res.avgTenDays;
      // this.barChartCon = res.conTenDays;
      // this.lineChartAvg = res.avgTenMonths;
      // this.lineChartCon = res.conTenMonths;
      this.createBarChart();
      this.createLineChart();
      this.createPieChart();
    });
  }

  ngOnChanges() {
    console.log(true);
  }

  createBarChart() {
    this.barChart = new Chart("MyBarChart", {
      type: "bar", //this denotes tha type of chart

      data: {
        // values on X-Axis
        labels: this.barChartLabels,
        datasets: [
          // {
          //   label: "Avarage",
          //   data: this.barChartAvg,
          //   backgroundColor: "blue",
          // },
          {
            label: "Consumed Avarage",
            data: this.barChartCon,
            backgroundColor: "limegreen",
          },
        ],
      },
      options: {
        aspectRatio: 2.5,
      },
    });
  }

  createPieChart() {
    this.pieChart = new Chart("MyPieChart", {
      type: "pie", //this denotes tha type of chart

      data: {
        labels: ["Red", "Blue", "Yellow"],
        datasets: [
          {
            label: "My First Dataset",
            data: [300, 50, 100],
            backgroundColor: [
              "rgb(255, 99, 132)",
              "rgb(54, 162, 235)",
              "rgb(255, 205, 86)",
            ],
            hoverOffset: 4,
          },
        ],
      },
      options: {
        aspectRatio: 2.5,
      },
    });
  }

  createLineChart() {
    this.lineChart = new Chart("MyLineChart", {
      type: "line", //this denotes tha type of chart

      data: {
        // values on X-Axis
        labels: this.lineChartLabels,
        datasets: [
          {
            label: "Avarage",
            data: this.lineChartAvg,
            backgroundColor: "blue",
            borderColor: "#248EDC",
            tension: 0.2,
          },
          {
            label: "Consumed",
            data: this.lineChartCon,
            backgroundColor: "limegreen",
            borderColor: "#7ADC24",
            tension: 0.2,
          },
        ],
      },
      options: {
        aspectRatio: 2.5,
      },
    });
  }

  getCurrentCard(payment: HTMLDivElement, payments: HTMLDivElement) {
    let count = 0;
    for (count; count < payments.children.length; count++) {
      payments.children[count].id = " ";
    }
    payment.id = "activeCard";
  }

  public updateChart(range: any) {
    const rangeValue = this.barChartLabels.slice(10 - range.value, 10);
    //const rangeAvgData = this.barChartAvg.slice(10 - range.value, 10);
    const rangeConData = this.barChartCon.slice(10 - range.value, 10);
    this.barChart.data.labels = rangeValue;
    //this.barChart.data.datasets[0].data = rangeAvgData;
    this.barChart.data.datasets[1].data = rangeConData;
    this.barChart.update();
  }

  selectOption(option: string) {
    this.selectedOption = option;
  }

  selectOption1(option: string) {
    this.selectedOption1 = option;
  }

  send(selectOption: string, selectOption1: string) {
    this._api
      .MeasurementPost(
        this.MeasurementType[selectOption],
        this.TimeRange[selectOption1],
        2
      )
      .subscribe((res) => {
        // console.log(res);
        this.barChartCon = [];
        this.barChartCon = res.map((item: any) => item.value);
        this.barChartLabels = [];
        this.barChartLabels = res.map((item: any) => item.time.slice(0, 10));
        this.barChart.data.datasets[1].data = this.barChartCon;
        this.barChart.data.labels = this.barChartLabels;
        this.barChart.update();
      });
  }
}
