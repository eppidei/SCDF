//
//  InAppiOS.h
//  ScdfController
//
//  Created by Marco Bertola on 2/04/15.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

#define kInAppPurchaseManagerProductsFetchedNotification @"kInAppPurchaseManagerProductsFetchedNotification"

#define kInAppPurchaseManagerTransactionFailedNotification @"kInAppPurchaseManagerTransactionFailedNotification"
#define kInAppPurchaseManagerTransactionSucceededNotification @"kInAppPurchaseManagerTransactionSucceededNotification"


@interface InAppPurchaseManager  : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
    int transactionsCount;
   
    SKProductsRequest *productsRequest;
    
    bool transactionSucceeded;
    bool userCancelledTransaction;
}

- ( void ) restoreInAppPurchases;
- ( void ) loadStore;
- ( BOOL ) canMakePurchases;
- ( void ) purchaseProUpgrade;
- ( void ) requestProUpgradeProductData;

@end