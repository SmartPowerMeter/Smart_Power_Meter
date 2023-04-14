import { Component, EventEmitter, OnInit, Output } from "@angular/core";
import { PaymentsService } from "../payments.service";
import Chart from "chart.js/auto";

@Component({
  selector: "app-payments-providers",
  templateUrl: "./payments-providers.component.html",
  styleUrls: ["./payments-providers.component.scss"],
})
export class PaymentsProvidersComponent implements OnInit {
  @Output() currentTransferWindow = new EventEmitter();
  paymentsArray: any;
  public chart: any;

  constructor(private paymentsService: PaymentsService) {}

  ngOnInit(): void {
    // this.paymentsService.getData().subscribe((payments) => {
    //   this.paymentsArray = this.paymentsService.data;
    // });
    this.createBarChart();
    this.createLineChart();
  }

  createBarChart() {
    this.chart = new Chart("MyBarChart", {
      type: "bar", //this denotes tha type of chart

      data: {
        // values on X-Axis
        labels: [
          "2022-05-10",
          "2022-05-11",
          "2022-05-12",
          "2022-05-13",
          "2022-05-14",
          "2022-05-15",
          "2022-05-16",
          "2022-05-17",
        ],
        datasets: [
          {
            label: "Sales",
            data: ["467", "576", "572", "79", "92", "574", "573", "576"],
            backgroundColor: "blue",
          },
          {
            label: "Profit",
            data: ["542", "542", "536", "327", "17", "0.00", "538", "541"],
            backgroundColor: "limegreen",
          },
        ],
      },
      options: {
        aspectRatio: 2.5,
      },
    });
  }

  createLineChart() {
    this.chart = new Chart("MyLineChart", {
      type: "line", //this denotes tha type of chart

      data: {
        // values on X-Axis
        labels: [
          "2022-05-10",
          "2022-05-11",
          "2022-05-12",
          "2022-05-13",
          "2022-05-14",
          "2022-05-15",
          "2022-05-16",
          "2022-05-17",
        ],
        datasets: [
          {
            label: "Sales",
            data: ["467", "576", "572", "79", "92", "574", "573", "576"],
            backgroundColor: "blue",
          },
          {
            label: "Profit",
            data: ["542", "542", "536", "327", "17", "0.00", "538", "541"],
            backgroundColor: "limegreen",
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

  getCurrentTransferName(event: any) {
    this.currentTransferWindow.emit(event);
  }
}
