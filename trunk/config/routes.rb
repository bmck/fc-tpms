# $Id$
# $(c)$

Rails.application.routes.draw do
  # The priority is based upon order of creation: first created -> highest priority.
  # See how all your routes lay out with "rake routes".

  devise_for :user, controllers: { sessions: "user/sessions", registrations: "user/registrations" }

  # You can have the root of your site routed with "root"
  # root 'welcome#index'

  root to: 'main#index'

  get '/errors/internal_server_error' => 'errors#internal_server_error'
  get '/errors/not_found' => 'errors#not_found'
  get '/errors/unprocessable_entity' => 'errors#unprocessable_entity'

  get '/home' => 'main#home'

  get '/tire_samples/create' => 'tire_samples#create'

  resources :companies, :users, :sensors, :receivers, :tires, :tire_types, :tire_samples, :trailers, :trucks, :storages do
    member do
      get 'view_audit'
    end
  end

  resources :tire_sample_report, only: [:new, :create]

  # Example of regular route:
  #   get 'products/:id' => 'catalog#view'

  # Example of named route that can be invoked with purchase_url(id: product.id)
  #   get 'products/:id/purchase' => 'catalog#purchase', as: :purchase

  # Example resource route (maps HTTP verbs to controller actions automatically):
  #   resources :products

  # Example resource route with options:
  #   resources :products do
  #     member do
  #       get 'short'
  #       post 'toggle'
  #     end
  #
  #     collection do
  #       get 'sold'
  #     end
  #   end

  # Example resource route with sub-resources:
  #   resources :products do
  #     resources :comments, :sales
  #     resource :seller
  #   end

  # Example resource route with more complex sub-resources:
  #   resources :products do
  #     resources :comments
  #     resources :sales do
  #       get 'recent', on: :collection
  #     end
  #   end

  # Example resource route with concerns:
  #   concern :toggleable do
  #     post 'toggle'
  #   end
  #   resources :posts, concerns: :toggleable
  #   resources :photos, concerns: :toggleable

  # Example resource route within a namespace:
  #   namespace :admin do
  #     # Directs /admin/products/* to Admin::ProductsController
  #     # (app/controllers/admin/products_controller.rb)
  #     resources :products
  #   end
end
