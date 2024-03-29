//
//  InAppiOS.mm
//  ScdfController
//
//  Created by Marco Bertola on 2/04/15.
//

#import "InAppiOS.h"
#include <string>
#include "SCDFCDefinitions.h"
#include "InAppPurchase.h"

#ifdef _DEBUG
#define SCDF_NOPURCHASE_NEEDED
#endif


// PRODUCT 2 index == 0
#define id_product_blow @"fzn_amidi_blow_00"
#define key_product_blow @"isAlgoBlow_Purchased"
#define message_product_blow @"Hey! Do you want to purchase the Blow control to use as MIDI output"

// PRODUCT 1 index == 1
#define id_product_saveLoad @"fzn_amidi_ls_00"
#define key_product_saveLoad @"isSaveLoad_Purchased"
#define message_product_saveLoad @"Hey! Do you want to purchase the Save and Load packet"

// PRODUCT 3 index == 2
#define id_product_snap @"fzn_amidi_snap_00"
#define key_product_snap @"isAlgoSnap_Purchased"
#define message_product_snap @"Hey! Do you want to purchase the Snap control to use as MIDI output"

// PRODUCT 4 index == 3
#define id_product_proxy @"fzn_amidi_proxy_00"
#define key_product_proxy @"isAlgoProxy_Purchased"
#define message_product_proxy @"Hey! Do you want to purchase the Proximity control to use as MIDI output"

//#define SCDF_TRANSACTION_RECEIPT @"proVersonTransactionReceipt"


//static std::string productPricePro;
//static SKProduct *productPro;



@interface ModalAlertDelegate : NSObject <UIAlertViewDelegate>
{
    CFRunLoopRef currentLoop;
    NSUInteger index;
}

@property (readonly) NSUInteger index;

@end


@implementation ModalAlertDelegate

@synthesize index;

- (void)alertView:(UIAlertView *)alertView willDismissWithButtonIndex:(NSInteger)buttonIndex
{
    
}// before animation and hiding view
- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex;
{

}
// Initialize with the supplied run loop
-(id) initWithRunLoop: (CFRunLoopRef)runLoop
{
    if (self = [super init]) currentLoop = runLoop;
    return self;
}

// User pressed button. Retrieve results
-(void) alertView: (UIAlertView*)aView clickedButtonAtIndex: (NSInteger)anIndex
{
    index = anIndex;
    CFRunLoopStop(currentLoop);
}

@end


void ShowModalAlert(std::string t)
{
    UIAlertView * alertView = [[UIAlertView alloc] initWithTitle:@"Controller" message:[NSString stringWithUTF8String:t.c_str()] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    
    [alertView show];
    [alertView release];
}


class InAppPurchaseController
{
    
    NSMutableArray *idList;
    NSMutableArray *keyList;
    NSMutableArray *messageList;
    
    
    NSMutableArray *productPricesPro;
    NSMutableArray *productsPro;
    
public:
    
    void initializeLists()
    {
        if(!productPricesPro)
        {
            productPricesPro = [[NSMutableArray alloc] init];
            productsPro = [[NSMutableArray alloc] init];
            
            
            // PRODUCT LIST:
            idList = [[NSMutableArray alloc] init];
            keyList = [[NSMutableArray alloc] init];
            messageList = [[NSMutableArray alloc] init];
            
            // IDs
            [idList addObject:id_product_blow];
            [idList addObject:id_product_saveLoad];
            [idList addObject:id_product_snap];
            [idList addObject:id_product_proxy];
            
            // KEY
            [keyList addObject:key_product_blow];
            [keyList addObject:key_product_saveLoad];
            [keyList addObject:key_product_snap];
            [keyList addObject:key_product_proxy];
            
            // MESSAGES
             [messageList addObject:message_product_blow];
            [messageList addObject:message_product_saveLoad];
            [messageList addObject:message_product_snap];
            [messageList addObject:message_product_proxy];
        }
        
    }
    
    NSMutableArray* GetProductsList()
    {
        return productsPro;
    }
    
    NSMutableArray* GetProductsPriceList()
    {
        return productPricesPro;
    }
    
    NSMutableArray* GetIdList()
    {
        return idList;
    }
    
    NSMutableArray* GetKeyList()
    {
        return keyList;
    }
    
    NSMutableArray* GetMessageList()
    {
        return messageList;
    }
    
    NSString *idAtIndex(int index)
    {
        return [idList objectAtIndex:index];
    }
    
    NSString *keyAtIndex(int index)
    {
        return [keyList objectAtIndex:index];
    }
    
    NSString *messageAtIndex(int index)
    {
        return [messageList objectAtIndex:index];
    }
    
private:
    InAppPurchaseManager *inAppManager;
    
    
    UIAlertView *purchaseActivityIndicator;
    
    void Cleanup()
    {
        if(purchaseActivityIndicator)
            [purchaseActivityIndicator dismissWithClickedButtonIndex: 0 animated:NO];
        purchaseActivityIndicator=NULL;
    }
    
public:

    
    InAppPurchaseController()
    {
        inAppManager=NULL;
    }
    
    void ShowModalProgress()
    {
        purchaseActivityIndicator = [[UIAlertView alloc] initWithTitle:@"Please wait..." message:nil delegate:nil cancelButtonTitle:nil otherButtonTitles:nil,nil];
        UIActivityIndicatorView *activity = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
        [activity setFrame:CGRectMake(121.0f, 55.0f, 36.0f, 36.0f)];
        [purchaseActivityIndicator addSubview:activity];
        [activity startAnimating];
        [activity release];
        [purchaseActivityIndicator show];
        [purchaseActivityIndicator release];
        return;
        
    }
    
    void PreloadStore()
    {
        
        initializeLists();
        
        if(inAppManager!=nil) return;
        inAppManager=[[InAppPurchaseManager alloc] init];
        [inAppManager loadStore];        
    }
    
    void TryRestoreInAppPurchases()
    {
        PreloadStore();
        if(!CanMakePurchases())
            return;
        [inAppManager restoreInAppPurchases];
    }
    
    bool CanMakePurchases()
    {
        if(![inAppManager canMakePurchases])
        {
            
            ShowModalAlert("Unable to purchase, please check that the internet connection is working and that you're logged into the App Store (via the device Settings app)");
            return false;
        }
        return true;
    }
    
    void Purchase(int productIndex)
    {
        
        PreloadStore();
        if(!CanMakePurchases())
            return;
        
        [inAppManager purchaseProUpgrade: productIndex];
    }
    
    void OnTransactionFinished(bool transactionSucceeded, bool userCancelledTransaction)
    {
        if(!transactionSucceeded) 
            if(!userCancelledTransaction)
                    ShowModalAlert("Purchase failed");
        
        Cleanup();

        if(transactionSucceeded)
        {
            // TODO: maybe we have to to something do laod the pro featuers
        }
        
    }
};



@implementation InAppPurchaseManager

static InAppPurchaseController purchaseController;

- (id) init
{
    self = [super init];
    if (self) {
        transactionSucceeded=false;
        userCancelledTransaction=false;
    }
    
    return self;
}

-(void) dealloc
{
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
    [super dealloc];
}


- (void) loadStore
{
    // restarts any purchases if they were interrupted last time the app was open
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    
    // get the product description (defined in early sections)
    [self requestProUpgradeProductData];
}

- (BOOL)canMakePurchases
{
    return [SKPaymentQueue canMakePayments];
}

- (void) restoreInAppPurchases
{
    transactionsCount=0;
    transactionSucceeded=false;
    userCancelledTransaction=false;
    
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{
    if(transactionsCount>0)
        ShowModalAlert("In-App purchase restored successfully");
    else ShowModalAlert("No purchases to restore were found");
}

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
    
    std::string errorStr = "";
    errorStr.append("In-App purchase restore failed. ");
    errorStr.append([[error localizedDescription] UTF8String]);
   
    ShowModalAlert(errorStr.c_str());
}

- (void)purchaseProUpgrade: (int ) productIndex
{
    if (!purchaseController.CanMakePurchases())
    {
        UIAlertView *alertView = [[UIAlertView alloc]initWithTitle:
                                  @"Purchases are disabled in your device" message:nil delegate:
                                  self cancelButtonTitle:@"Ok" otherButtonTitles: nil];
        [alertView show];
        [alertView release];
        return;
    }
    
    transactionsCount=0;
    transactionSucceeded=false;
    userCancelledTransaction=false;

    NSMutableArray *productList = purchaseController.GetProductsList();
    if(![productList count])
    {
        UIAlertView *alertView = [[UIAlertView alloc]initWithTitle:
                                  @"The product is not available" message:nil delegate:
                                  self cancelButtonTitle:@"Ok" otherButtonTitles: nil];
        [alertView show];
        [alertView release];
        return;
    }
    
    SKProduct *selectedProduct = [purchaseController.GetProductsList() objectAtIndex:productIndex];
    
    if (nil==selectedProduct)
    {
        UIAlertView *alertView = [[UIAlertView alloc]initWithTitle:
                                  @"The product is not available" message:nil delegate:
                                  self cancelButtonTitle:@"Ok" otherButtonTitles: nil];
        [alertView show];
        [alertView release];
        return;
    }
    
    SKPayment *payment = [SKPayment paymentWithProduct:selectedProduct];
    
    [[SKPaymentQueue defaultQueue] addPayment:payment];
    
    //SKPayment *payment = [SKPayment paymentWithProductIdentifier:kInAppPurchaseProUpgradeProductId];
    //[[SKPaymentQueue defaultQueue] addPayment:payment];
}

- (void)requestProUpgradeProductData
{
    NSArray *list = [NSArray arrayWithArray:purchaseController.GetIdList()];
    
    NSSet *productIdentifiers = [NSSet setWithArray:list];
    
    

    
    //NSSet *productIdentifiers = [NSSet setWithObjects:kInAppPurchaseProUpgradeProductId, nil ];

    productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];
    productsRequest.delegate = self;
    [productsRequest start];
    
}

- (void)recordTransaction:(SKPaymentTransaction *)transaction
{
    NSString *productId=transaction.payment.productIdentifier;
    
    
    NSMutableArray *idList = purchaseController.GetIdList();
    
    for (NSString * idValue in idList) {
        
        if ([productId isEqualToString:idValue])
        {
            transactionsCount++;
            // save the transaction receipt to disk
            [[NSUserDefaults standardUserDefaults] setValue:transaction.transactionReceipt forKey:idValue ];
            [[NSUserDefaults standardUserDefaults] synchronize];
        }
        
    }

}

- (void)provideContent:(NSString *)productId
{
    //[[NSUserDefaults standardUserDefaults] setBool:YES forKey:SCDF_INAPP_DEFAULTS_KEY];
    //[[NSUserDefaults standardUserDefaults] synchronize];
    
    NSMutableArray *idList = purchaseController.GetIdList();
    NSMutableArray *keyList = purchaseController.GetKeyList();
    
    for (NSString * idValue in idList) {
        
        if ([productId isEqualToString:idValue])
        {
            [[NSUserDefaults standardUserDefaults] setBool:YES forKey:[keyList objectAtIndex:[idList indexOfObject:idValue]]];
            [[NSUserDefaults standardUserDefaults] synchronize];
        }
        
    }

}

-(void)onTransactionFinished
{
    purchaseController.OnTransactionFinished(transactionSucceeded, userCancelledTransaction);
}

- (void)finishTransaction:(SKPaymentTransaction *)transaction wasSuccessful:(BOOL)wasSuccessful
{
    // remove the transaction from the payment queue.
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    
    NSDictionary *userInfo = [NSDictionary dictionaryWithObjectsAndKeys:transaction, @"transaction" , nil];
    if (wasSuccessful)
    {
        // send out a notification that we’ve finished the transaction
        [[NSNotificationCenter defaultCenter] postNotificationName:kInAppPurchaseManagerTransactionSucceededNotification object:self userInfo:userInfo];
        transactionSucceeded=true;
    }
    else
    {
        // send out a notification for the failed transaction
        [[NSNotificationCenter defaultCenter] postNotificationName:kInAppPurchaseManagerTransactionFailedNotification object:self userInfo:userInfo];
        transactionSucceeded=false;
    }
    [self performSelectorOnMainThread:@selector(onTransactionFinished) withObject:self waitUntilDone:NO];
}

- (void)completeTransaction:(SKPaymentTransaction *)transaction
{
    [self recordTransaction:transaction];
    [self provideContent:transaction.payment.productIdentifier];
    [self finishTransaction:transaction wasSuccessful:YES];
}

- (void)restoreTransaction:(SKPaymentTransaction *)transaction
{
    [self recordTransaction:transaction.originalTransaction];
    [self provideContent:transaction.originalTransaction.payment.productIdentifier];
    [self finishTransaction:transaction wasSuccessful:YES];
}

- (void)failedTransaction:(SKPaymentTransaction *)transaction
{
    if (transaction.error.code != SKErrorPaymentCancelled)
    {
        // error!
        [self finishTransaction:transaction wasSuccessful:NO];
    }
    else
    {
        self->userCancelledTransaction=true;
        
        // this is fine, the user just cancelled, so don’t notify
        [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
        
        [self finishTransaction:transaction wasSuccessful:NO];
    }
}

#pragma mark -
#pragma mark SKPaymentTransactionObserver methods

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchasing:
                purchaseController.ShowModalProgress();
                break;
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
                break;
            default:
                break;
        }
    }
}
#pragma mark -
#pragma mark SKProductsRequestDelegate methods



- (NSString *) priceAsString: (SKProduct *)product
{
    NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
    [formatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [formatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    [formatter setLocale:[product priceLocale]];
    
    NSString *str = [formatter stringFromNumber:[product price]];
    [formatter release];
    return str;
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    NSLog(@"request:(SKRequest *)request didFailWithError:(NSError *)error");
}
- (void)requestDidFinish:(SKRequest *)request
{
    NSLog(@"requestDidFinish:(SKRequest *)request");
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{

    NSArray *products = response.products;

    for (SKProduct *proUpgradeProduct in products)
    {
#ifdef _DEBUG
        NSLog(@"Product title: %@" , proUpgradeProduct.localizedTitle);
        NSLog(@"Product description: %@" , proUpgradeProduct.localizedDescription);
        NSLog(@"Product price: %@" , proUpgradeProduct.price);
        NSLog(@"Product id: %@" , proUpgradeProduct.productIdentifier);
        NSLog(@"--");
#endif
        
        [purchaseController.GetProductsList() addObject:proUpgradeProduct];
        [purchaseController.GetProductsPriceList()addObject:[self priceAsString: proUpgradeProduct]];
        
        //productPricePro=[[self priceAsString: proUpgradeProduct] UTF8String];
        //productPro = proUpgradeProduct;
    }

    for (NSString *invalidProductId in response.invalidProductIdentifiers)
    {
#ifdef _DEBUG
        NSLog(@"Invalid product id: %@" , invalidProductId);
#endif
       
    }
    
    // finally release the reqest we alloc/init’ed in requestProUpgradeProductData
    [productsRequest release];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:kInAppPurchaseManagerProductsFetchedNotification object:self userInfo:nil];
}


@end

void PreloadStore()
{
    purchaseController.PreloadStore();
}

int GetRealIndex(PurchaseProductIndex productIndex)
{
    switch(productIndex)
    {
        case PurchaseProductIndex_saveload: return 1;
        case PurchaseProductIndex_blow: return 0;
        default: return productIndex;
    }
    return productIndex;
}

bool CheckIsInAppPurchasedNoPrompt(PurchaseProductIndex _productIndex)
{
#ifdef SCDF_NOPURCHASE_NEEDED
    return true;
#endif
    int productIndex=GetRealIndex(_productIndex);
    if([[NSUserDefaults standardUserDefaults] boolForKey: purchaseController.keyAtIndex(productIndex)]) return true;
    return false;
}


bool CheckIsInAppPurchased(PurchaseProductIndex _productIndex)
{
#ifdef SCDF_NOPURCHASE_NEEDED
    return true;
#endif
    int productIndex=GetRealIndex(_productIndex);
    if([[NSUserDefaults standardUserDefaults] boolForKey: purchaseController.keyAtIndex(productIndex)])
        return true;
    
    NSString *message = purchaseController.messageAtIndex(productIndex);
    std::string msg= [message UTF8String];
    
    NSMutableArray *priceList = purchaseController.GetProductsPriceList();
    
    if([priceList count]>productIndex)
    {
        std::string productPrice = [[priceList objectAtIndex:productIndex] UTF8String];
        if(productPrice.length()>0)
        {
            msg.append(" for ");
            msg.append(productPrice);
            msg.append("?");
        }
    }
    
    
    CFRunLoopRef currentLoop = CFRunLoopGetCurrent();
    
    // Create Alert

    ModalAlertDelegate *madelegate = [[ModalAlertDelegate alloc] initWithRunLoop:currentLoop];
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:[NSString stringWithUTF8String:msg.c_str()] message:nil delegate:madelegate cancelButtonTitle:@"NO" otherButtonTitles:@"YES", nil, nil];
    [alertView show];
   
    alertView.hidden=NO;
    
    // Wait for response
    
    CFRunLoopRun();
    
    // Retrieve answer
    
    NSUInteger answer = madelegate.index;
    
    if(0!=answer)
    {
         purchaseController.Purchase(productIndex);
    }
    
    [alertView release];
    return false;
}


bool TryRestoreInAppPurchases()
{
    purchaseController.TryRestoreInAppPurchases();
    return true;
}