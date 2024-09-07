// swift-tools-version: 5.10

import PackageDescription

let package = Package(
    name: "NMapsMap",
    platforms: [
      .iOS(.v12)
    ],
    products: [
        .library(
            name: "NMapsMap",
            targets: ["NMapsMap"]),
    ],
    targets: [
        .binaryTarget(
          name: "NMapsMap",
          path: "framework/NMapsMap.xcframework"
        )
    ]
)
