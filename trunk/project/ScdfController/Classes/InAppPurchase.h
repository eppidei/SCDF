/*
 * InAppPurchase.h
*/

#ifndef _SCDFC_IN_APP_PURCHASE_H_
#define _SCDFC_IN_APP_PURCHASE_H_

//enum mirrored with ControlUnit type enum
//In ControlUnit type enum 0 is Wire, that isn't under purchase...so we can use 0 index for saveload purchase
enum PurchaseProductIndex { PurchaseProductIndex_saveload, PurchaseProductIndex_blow, PurchaseProductIndex_snap};

bool CheckIsInAppPurchased(PurchaseProductIndex index);
bool CheckIsInAppPurchasedNoPrompt(PurchaseProductIndex index);


#endif
