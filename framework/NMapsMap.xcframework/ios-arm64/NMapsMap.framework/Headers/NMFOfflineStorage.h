#import <Foundation/Foundation.h>

#import "NMFFoundation.h"
#import "NMFTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class NMFOfflinePack;
@class NMGLatLngBounds;
@protocol NMFOfflineRegion;
@protocol NMFOfflineStorageDelegate;

/**
 Posted by the shared `NMFOfflineStorage` object when an `NMFOfflinePack`
 object’s progress changes. The progress may change due to a resource being
 downloaded or because the pack discovers during the download that more
 resources are required for offline viewing. This notification is posted
 whenever any field in the `progress` property changes.
 
 The `object` is the `NMFOfflinePack` object whose progress changed. The
 `NMFOfflinePackUserInfoKeyState` key and details about the pack’s current
 progress in the `NMFOfflinePackUserInfoKeyProgress` key. You may also consult
 progress in the `NMFOfflinePackProgressUserInfoKey` key. You may also consult
 the `NMFOfflinePack.state` and `NMFOfflinePack.progress` properties, which
 provide the same values.
 
 If you only need to observe changes in a particular pack’s progress, you can
 alternatively observe KVO change notifications to the pack’s `progress` key
 path.
 
 #### Related examples
 See the <a href="https://docs.mapbox.com/ios/maps/examples/offline-pack/">
 Download an offline map</a> example to learn how to calculate the progress
 of an offline download.
 */
extern NMF_EXPORT const NSNotificationName NMFOfflinePackProgressChangedNotification;

/**
 Posted by the shared `NMFOfflineStorage` object whenever an `NMFOfflinePack`
 object encounters an error while downloading. The error may be recoverable and
 may not warrant the user’s attention. For example, the pack’s implementation
 may attempt to re-request failed resources based on an exponential backoff
 strategy or upon the restoration of network access.

 The `object` is the `NMFOfflinePack` object that encountered the error. The
 `userInfo` dictionary contains the error object in the
 `NMFOfflinePackUserInfoKeyError` key.
 */
extern NMF_EXPORT const NSNotificationName NMFOfflinePackErrorNotification;

/**
 Posted by the shared `NMFOfflineStorage` object when the maximum number of
 Mapbox-hosted tiles has been downloaded and stored on the current device.

 The `object` is the `NMFOfflinePack` object that reached the tile limit in the
 course of downloading. The `userInfo` dictionary contains the tile limit in the
 `NMFOfflinePackUserInfoKeyMaximumCount` key.

 Once this limit is reached, no instance of `NMFOfflinePack` can download
 additional tiles from Mapbox APIs until already downloaded tiles are removed by
 calling the `-[NMFOfflineStorage removePack:withCompletionHandler:]` method.
 */
extern NMF_EXPORT const NSNotificationName NMFOfflinePackMaximumTilesReachedNotification;

/**
 A key in the `userInfo` property of a notification posted by `NMFOfflinePack`.
 */
typedef NSString *NMFOfflinePackUserInfoKey NS_EXTENSIBLE_STRING_ENUM;

/**
 The key for an `NSNumber` object that indicates an offline pack’s current
 state. This key is used in the `userInfo` dictionary of an
 `NMFOfflinePackProgressChangedNotification` notification. Call `-integerValue`
 on the object to receive the `NMFOfflinePackState`-typed state.
 */
extern NMF_EXPORT const NMFOfflinePackUserInfoKey NMFOfflinePackUserInfoKeyState;

/**
 The key for an `NSValue` object that indicates an offline pack’s current
 progress. This key is used in the `userInfo` dictionary of an
 `NMFOfflinePackProgressChangedNotification` notification. Call
 `-NMFOfflinePackProgressValue` on the object to receive the
 `NMFOfflinePackProgress`-typed progress.
 */
extern NMF_EXPORT const NMFOfflinePackUserInfoKey NMFOfflinePackUserInfoKeyProgress;

/**
 The key for an `NSError` object that is encountered in the course of
 downloading an offline pack. This key is used in the `userInfo` dictionary of
 an `NMFOfflinePackErrorNotification` notification. The error’s domain is
 `NMFErrorDomain`. See `NMFErrorCode` for possible error codes.
 */
extern NMF_EXPORT const NMFOfflinePackUserInfoKey NMFOfflinePackUserInfoKeyError;

/**
 The key for an `NSNumber` object that indicates the maximum number of
 Mapbox-hosted tiles that may be downloaded and stored on the current device.
 This key is used in the `userInfo` dictionary of an
 `NMFOfflinePackMaximumTilesReachedNotification` notification. Call
 `-unsignedLongLongValue` on the object to receive the `uint64_t`-typed tile
 limit.
 */
extern NMF_EXPORT const NMFOfflinePackUserInfoKey NMFOfflinePackUserInfoKeyMaximumCount;

extern NMF_EXPORT NSString * const NMFOfflinePackMaximumCountUserInfoKey __attribute__((deprecated("Use NMFOfflinePackUserInfoKeyMaximumCount")));

extern NMF_EXPORT NMFExceptionName const NMFUnsupportedRegionTypeException;

/**
 A block to be called once an offline pack has been completely created and
 added.

 An application typically calls the `-resume` method on the pack inside this
 completion handler to begin the download.

 @param pack Contains a pointer to the newly added pack, or `nil` if there was
    an error creating or adding the pack.
 @param error Contains a pointer to an error object (if any) indicating why the
    pack could not be created or added.
 */
typedef void (^NMFOfflinePackAdditionCompletionHandler)(NMFOfflinePack * _Nullable pack, NSError * _Nullable error);

/**
 A block to be called once an offline pack has been completely invalidated and
 removed.

 Avoid any references to the pack inside this completion handler: by the time
 this completion handler is executed, the pack has become invalid, and any
 messages passed to it will raise an exception.

 @param error Contains a pointer to an error object (if any) indicating why the
    pack could not be invalidated or removed.
 */
typedef void (^NMFOfflinePackRemovalCompletionHandler)(NSError * _Nullable error);

typedef void (^NMFOfflinePackCompletionHandler)(void);

/**
 A block to be called once the contents of a file are copied into the current packs.
 
 @param fileURL The file URL of the offline database containing the offline packs
 that were copied.
 @param packs An array of all known offline packs, or `nil` if there was an error
 creating or adding the pack.
 @param error A pointer to an error object (if any) indicating why the pack could
 not be created or added.
 */
typedef void (^NMFBatchedOfflinePackAdditionCompletionHandler)(NSURL *fileURL, NSArray<NMFOfflinePack *> * _Nullable packs, NSError * _Nullable error);

/**
 The type of resource that is requested.
 */
typedef NS_ENUM(NSUInteger, NMFResourceKind) {
    /** Unknown type */
    NMFResourceKindUnknown,
    /** Style sheet JSON file */
    NMFResourceKindStyle,
    /** TileJSON file as specified in https://www.mapbox.com/mapbox-gl-js/style-spec/#root-sources */
    NMFResourceKindSource,
    /** A vector or raster tile as described in the style sheet at
        https://www.mapbox.com/mapbox-gl-js/style-spec/#sources */
    NMFResourceKindTile,
    /** Signed distance field glyphs for text rendering. These are the URLs specified in the style
        in https://www.mapbox.com/mapbox-gl-js/style-spec/#root-glyphs */
    NMFResourceKindGlyphs,
    /** Image part of a sprite sheet. It is constructed of the prefix in
        https://www.mapbox.com/mapbox-gl-js/style-spec/#root-sprite and a PNG file extension. */
    NMFResourceKindSpriteImage,
    /** JSON part of a sprite sheet. It is constructed of the prefix in
        https://www.mapbox.com/mapbox-gl-js/style-spec/#root-sprite and a JSON file extension. */
    NMFResourceKindSpriteJSON,
    /** Image data for a georeferenced image source. **/
    NMFResourceKindImage,
};

/**
 NMFOfflineStorage implements a singleton (shared object) that manages offline
 packs and ambient caching. All of this class’s instance methods are asynchronous,
 reflecting the fact that offline resources are stored in a database. The shared
 object maintains a canonical collection of offline packs in its `packs` property.
 
 #### Related examples
 See the <a href="https://docs.mapbox.com/ios/maps/examples/offline-pack/">
 Download an offline map</a> example to learn how to create and register an
 offline pack for a defined region.
 */
NMF_EXPORT
@interface NMFOfflineStorage : NSObject

/**
 Returns the shared offline storage object.
 */
@property (class, nonatomic, readonly) NMFOfflineStorage *sharedOfflineStorage;

#pragma mark - Adding Contents of File

/**
 Adds the offline packs located at the given file path to offline storage.
 
 The file must be a valid offline region database bundled with the application
 or downloaded separately.
 
 The resulting packs are added or updated to the shared offline storage object’s `packs`
 property, then the `completion` block is executed.
 
 @param filePath A string representation of the file path. The file path must be
 writable as schema updates may be perfomed.
 @param completion The completion handler to call once the contents of the given
 file has been added to offline storage. This handler is executed asynchronously
 on the main queue.
 */
- (void)addContentsOfFile:(NSString *)filePath withCompletionHandler:(nullable NMFBatchedOfflinePackAdditionCompletionHandler)completion;

/**
 Adds the offline packs located at the given URL to offline storage.
 
 The file must be a valid offline region database bundled with the application
 or downloaded separately.
 
 The resulting packs are added or updated to the shared offline storage object’s `packs`
 property, then the `completion` block is executed.
 
 @param fileURL A file URL specifying the file to add. URL should be a valid system path.
 The file URL must be writable as schema updates may be performed.
 @param completion The completion handler to call once the contents of the given
 file has been added to offline storage. This handler is executed asynchronously
 on the main queue.
 */
- (void)addContentsOfURL:(NSURL *)fileURL withCompletionHandler:(nullable NMFBatchedOfflinePackAdditionCompletionHandler)completion;

#pragma mark - Accessing the Delegate

/**
 The receiver’s delegate.

 An offline storage object sends messages to its delegate to allow it to
 transform URLs before they are requested from the internet. This can be used
 add or remove custom parameters, or reroute certain requests to other servers
 or endpoints.
 */
@property(nonatomic, weak, nullable) IBOutlet id<NMFOfflineStorageDelegate> delegate;

#pragma mark - Managing Offline Packs

/**
 An array of all known offline packs, in the order in which they were created.

 This property is set to `nil`, indicating that the receiver does not yet know
 the existing packs, for an undefined amount of time starting from the moment
 the shared offline storage object is initialized until the packs are fetched
 from the database. After that point, this property is always non-nil, but it
 may be empty to indicate that no packs are present.

 To detect when the shared offline storage object has finished loading its
 `packs` property, observe KVO change notifications on the `packs` key path.
 The initial load results in an `NSKeyValueChangeSetting` change.
 */
@property (nonatomic, strong, readonly, nullable) NSArray<NMFOfflinePack *> *packs;

/**
 Creates and registers an offline pack that downloads the resources needed to
 use the given region offline.

 The resulting pack is added to the shared offline storage object’s `packs`
 property, then the `completion` block is executed with that pack passed in.

 The pack has an initial state of `NMFOfflinePackStateInactive`. To begin
 downloading resources, call `-[NMFOfflinePack resume]` on the pack from within
 the completion handler. To monitor download progress, add an observer for
 `NMFOfflinePackProgressChangedNotification`s about that pack.

 To detect when any call to this method results in a new pack, observe KVO
 change notifications on the shared offline storage object’s `packs` key path.
 Additions to that array result in an `NSKeyValueChangeInsertion` change.

 @param region A region to download.
 @param context Arbitrary data to store alongside the downloaded resources.
 @param completion The completion handler to call once the pack has been added.
    This handler is executed asynchronously on the main queue.
 */
- (void)addPackForRegion:(id <NMFOfflineRegion>)region withContext:(NSData *)context completionHandler:(nullable NMFOfflinePackAdditionCompletionHandler)completion;

/**
 Unregisters the given offline pack and allows resources that are no longer
 required by any remaining packs to be potentially freed.

 As soon as this method is called on a pack, the pack becomes invalid; any
 attempt to send it a message will result in an exception being thrown. If an
 error occurs and the pack cannot be removed, do not attempt to reuse the pack
 object. Instead, if you need continued access to the pack, suspend all packs
 and use the `-reloadPacks` method to obtain valid pointers to all the packs.

 To detect when any call to this method results in a pack being removed, observe
 KVO change notifications on the shared offline storage object’s `packs` key
 path. Removals from that array result in an `NSKeyValueChangeRemoval` change.

 When you remove an offline pack, any resources that are required by that pack,
 but not other packs, become eligible for deletion from offline storage. Because
 the backing store used for offline storage is also used as a general purpose
 cache for map resources, such resources may not be immediately removed if the
 implementation determines that they remain useful for general performance of
 the map.

 @param pack The offline pack to remove.
 @param completion The completion handler to call once the pack has been
    removed. This handler is executed asynchronously on the main queue.
 */
- (void)removePack:(NMFOfflinePack *)pack withCompletionHandler:(nullable NMFOfflinePackRemovalCompletionHandler)completion;

/**
 Invalidates the specified offline pack. This method checks that the tiles
 in the specified offline pack match those from the server. Local tiles that
 do not match the latest version on the server are updated.
 
 This is more efficient than deleting the offline pack and downloading it
 again. If the data stored locally matches that on the server, new data will
 not be downloaded.
 
 @param pack The offline pack to be invalidated.
 @param completion The completion handler to call once the pack has been
 removed. This handler is executed asynchronously on the main queue.
 */
- (void)invalidatePack:(NMFOfflinePack *)pack withCompletionHandler:(void (^)(NSError * _Nullable))completion;
/**
 Forcibly, asynchronously reloads the `packs` property. At some point after this
 method is called, the pointer values of the `NMFOfflinePack` objects in the
 `packs` property change, even if the underlying data for these packs has not
 changed. If this method is called while a pack is actively downloading, the
 behavior is undefined.

 You typically do not need to call this method.

 To detect when the shared offline storage object has finished reloading its
 `packs` property, observe KVO change notifications on the `packs` key path.
 A reload results in an `NSKeyValueChangeSetting` change.
 */
- (void)reloadPacks;

/**
 파일 캐시 초기화
 */
- (void)flushCacheWithCompletionHandler:(nullable NMFOfflinePackRemovalCompletionHandler)completion;

#pragma mark Preload Database

- (void)addPreloadTile:(NSString *)filePath
              sourceId:(NSString *)sourceId
         tileUrlPrefix:(NSString *)tileUrlPrefix
           tileVersion:(NSString *)tileVersion
               minZoom:(NSInteger)minzoom
               maxZoom:(NSInteger)maxZoom
                bounds:(NMGLatLngBounds *)bounds
            completion:(nullable NMFOfflinePackCompletionHandler)completion;

- (void)removePreloadTile:(NSString *)filePath
               completion:(nullable NMFOfflinePackCompletionHandler)completion;

- (void)setPreloadFallbackToStreaming:(BOOL)preloadFallbackToStreaming
                           completion:(nullable NMFOfflinePackCompletionHandler)completion;

/**
 The cumulative size, measured in bytes, of all downloaded resources on disk.
 
 The returned value includes all resources, including tiles, whether downloaded
 as part of an offline pack or due to caching during normal use of `NMFMapView`.
 */
@property (nonatomic, readonly) unsigned long long countOfBytesCompleted;

#pragma mark - Managing Ambient Cache

/**
 Sets the maximum ambient cache size in bytes. The default maximum cache
 size is 50 MB. To disable ambient caching, set the maximum ambient cache size
 to `0`. Setting the maximum ambient cache size does not impact the maximum size
 of offline packs.
 
 While this method does not limit the space available to offline packs,
 data in offline packs count towards this limit. If the maximum ambient
 cache size is set to 30 MB and 20 MB of offline packs are downloaded,
 there may be only 10 MB reserved for the ambient cache.
 
 This method should be called before the map and map style have been loaded.
 
 This method is potentially expensive, as the database will trim cached data
 in order to prevent the ambient cache from being larger than the
 specified amount.
 
 @param cacheSize The maximum size in bytes for the ambient cache.
 @param completion The completion handler to call once the maximum ambient cache size
 has been set. This handler is executed synchronously on the main queue.
 */

- (void)setMaximumAmbientCacheSize:(NSUInteger)cacheSize withCompletionHandler:(void (^)(NSError *_Nullable error))completion;

/**
 Invalidates the ambient cache. This method checks that the tiles in the
 ambient cache match those from the server. If the local tiles do not match
 those on the server, they are re-downloaded.
 
 This is recommended over clearing the cache or resetting the database
 because valid local tiles will not be downloaded again.
 
 Resources shared with offline packs will not be affected by this method.
 
 @param completion The completion handler to call once the ambient cache has
 been revalidated. This handler is executed asynchronously on the main queue.
 */

- (void)invalidateAmbientCacheWithCompletionHandler:(void (^)(NSError *_Nullable error))completion;

/**
 Clears the ambient cache by deleting resources. This method does not
 affect resources shared with offline regions.
 
 @param completion The completion handler to call once resources from
 the ambient cache have been cleared. This handler is executed
 asynchronously on the main queue.
 */

- (void)clearAmbientCacheWithCompletionHandler:(void (^)(NSError *_Nullable error))completion;

/**
 Deletes the existing database, which includes both the ambient cache and offline packs,
 then reinitializes it.
 
 You typically do not need to call this method.
 
 @param completion The completion handler to call once the pack has database has
 been reset. This handler is executed asynchronously on the main queue.
 */

- (void)resetDatabaseWithCompletionHandler:(void (^)(NSError *_Nullable error))completion;

/*
 Inserts the provided resource into the ambient cache.
 
 This method mimics the caching that would take place if the equivalent resource
 were requested in the process of map rendering. Use this method to pre-warm the
 cache with resources you know will be requested.
 
 This method is asynchronous; the data may not be immediately available for
 in-progress requests, though subsequent requests should have access to the
 cached data.
 
 @param data Response data to store for this resource. The data is expected to
 be uncompressed; internally, the cache will compress data as necessary.
 @param url The URL at which the data can normally be found.
 @param modified The date the resource was last modified.
 @param expires The date after which the resource is no longer valid.
 @param eTag An HTTP entity tag.
 @param mustRevalidate A Boolean value indicating whether the data is still
 usable past the expiration date.
 */
- (void)preloadData:(NSData *)data forURL:(NSURL *)url modificationDate:(nullable NSDate *)modified expirationDate:(nullable NSDate *)expires eTag:(nullable NSString *)eTag mustRevalidate:(BOOL)mustRevalidate NS_SWIFT_NAME(preload(_:for:modifiedOn:expiresOn:eTag:mustRevalidate:));

- (void)putResourceWithUrl:(NSURL *)url data:(NSData *)data modified:(nullable NSDate *)modified expires:(nullable NSDate *)expires etag:(nullable NSString *)etag mustRevalidate:(BOOL)mustRevalidate __attribute__((deprecated("", "-preloadData:forURL:modificationDate:expirationDate:eTag:mustRevalidate:")));

@end

/**
 The `NMFOfflineStorageDelegate` protocol defines methods that a delegate of an
 `NMFOfflineStorage` object can optionally implement to transform various types
 of URLs before downloading them via the internet.
 */
@protocol NMFOfflineStorageDelegate <NSObject>

/**
 Sent whenever a URL needs to be transformed.

 @param storage The storage object processing the download.
 @param kind The kind of URL to be transformed.
 @param url The original URL to be transformed.
 @return A URL that will now be downloaded.
 */
- (NSURL *)offlineStorage:(NMFOfflineStorage *)storage
     URLForResourceOfKind:(NMFResourceKind)kind
                  withURL:(NSURL *)url;

@end

NS_ASSUME_NONNULL_END
